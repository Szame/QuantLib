/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2007 Chris Kenyon

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file inflationindex.hpp
    \brief base classes for inflation indexes
*/

#ifndef quantlib_inflation_index_hpp
#define quantlib_inflation_index_hpp

#include <ql/index.hpp>
#include <ql/indexes/region.hpp>
#include <ql/currency.hpp>
#include <ql/handle.hpp>

namespace QuantLib {

    class ZeroInflationTermStructure;
    class YoYInflationTermStructure;

    //! Base class for inflation-rate indexes,
    class InflationIndex : public Index, public Observer {
      public:
        InflationIndex(const std::string& familyName,
                       const Region& region,
                       bool revised,
                       bool interpolated,
                       Frequency frequency,
                       const Period& availabilitiyLag,
                       const Currency& currency);
        //! \name Index interface
        //@{
        std::string name() const;

        /*! Inflation indices do not have fixing calendars.  An
            inflation index value is valid for every day (including
            weekends) of a calendar period.  I.e. it uses the
            NullCalendar as its fixing calendar.
        */
        Calendar fixingCalendar() const;
        bool isValidFixingDate(const Date& fixingDate) const;

        /*! Forecasting index values requires an inflation term
            structure.  The inflation term structure (ITS) defines the
            usual lag (not the index).  I.e.  an ITS is always relatve
            to a base date that is earlier than its asof date.  This
            must be so because indices are available only with a lag.
            However, the index availability lag only sets a minimum
            lag for the ITS.  An ITS may be relative to an earlier
            date, e.g. an index may have a 2-month delay in
            publication but the inflation swaps may take as their base
            the index 3 months before.
        */
        virtual Rate fixing(const Date& fixingDate,
                            bool forecastTodaysFixing = false) const = 0;

        /*! this method creates all the "fixings" for the relevant
            period of the index.  E.g. for monthly indices it will put
            the same value in every calendar day in the month.
        */
        void addFixing(const Date& fixingDate,
                       Rate fixing,
                       bool forceOverwrite = false);
        //@}

        //! \name Observer interface
        //@{
        void update();
        //@}

        //! \name Inspectors
        //@{
        std::string familyName() const;
        Region region() const;
        bool revised() const;
        /*! Forecasting index values using an inflation term structure
            uses the interpolation of the inflation term structure
            unless interpolation is set to false.  In this case the
            extrapolated values are constant within each period taking
            the mid-period extrapolated value.
        */
        bool interpolated() const;
        Frequency frequency() const;
        /*! The availability lag describes when the index is
            <i>available</i>, not how it is used.  Specifically the
            fixing for, say, January, may only be available in April
            but the index will always return the index value
            applicable for January as its January fixing (independent
            of the lag in availability).
        */
        Period availabilityLag() const;
        Currency currency() const;
        //@}

      protected:
        Date referenceDate_;
        std::string familyName_;
        Region region_;
        bool revised_;
        bool interpolated_;
        Frequency frequency_;
        Period availabilityLag_;
        Currency currency_;
    };


    //! Base class for zero inflation indices.
    class ZeroInflationIndex : public InflationIndex {
      public:
        //! Always use the evaluation date as the reference date
        ZeroInflationIndex(const std::string& familyName,
                           const Region& region,
                           bool revised,
                           bool interpolated,
                           Frequency frequency,
                           const Period& availabilityLag,
                           const Currency& currency,
                           const Handle<ZeroInflationTermStructure>& ts =
                                        Handle<ZeroInflationTermStructure>());

        Rate fixing(const Date& fixingDate,
                    bool forecastTodaysFixing = false) const;

        Handle<ZeroInflationTermStructure> zeroInflationTermStructure() const;
      private:
        Rate forecastFixing(const Date& fixingDate) const;
        Handle<ZeroInflationTermStructure> zeroInflation_;
    };

    //! Base class for year-on-year inflation indices.
    /*! These may be genuine indices published on, say, Bloomberg, or
        "fake" indices that are defined as the ratio of an index at
        different time points.
    */
    class YoYInflationIndex : public InflationIndex {
      public:
        YoYInflationIndex(const std::string& familyName,
                          const Region& region,
                          bool revised,
                          bool interpolated,
                          bool ratio, // is this one a genuine index or a ratio?
                          Frequency frequency,
                          const Period& availabilityLag,
                          const Currency& currency,
                          const Handle<YoYInflationTermStructure>& ts =
                                         Handle<YoYInflationTermStructure>());

        Rate fixing(const Date& fixingDate,
                    bool forecastTodaysFixing = false) const;

        bool ratio() const;
        Handle<YoYInflationTermStructure> yoyInflationTermStructure() const;
      private:
        Rate forecastFixing(const Date& fixingDate) const;
        bool ratio_;
        Handle<YoYInflationTermStructure> yoyInflation_;
    };

}


#endif
