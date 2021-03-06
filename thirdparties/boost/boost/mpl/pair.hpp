
#ifndef BOOST_MPL_PAIR_HPP_INCLUDED
#define BOOST_MPL_PAIR_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: pair.hpp 85945 2013-09-26 09:46:46Z skelly $
// $Date: 2013-09-26 18:46:46 +0900 (목, 26 9 2013) $
// $Revision: 85945 $

#include <boost/mpl/aux_/na_spec.hpp>
#include <boost/mpl/aux_/lambda_support.hpp>

namespace boost { namespace mpl {

template<
      typename BOOST_MPL_AUX_NA_PARAM(T1)
    , typename BOOST_MPL_AUX_NA_PARAM(T2)
    >
struct pair
{
    typedef pair type;
    typedef T1 first;
    typedef T2 second;

    BOOST_MPL_AUX_LAMBDA_SUPPORT(2,pair,(T1,T2))
};

template<
      typename BOOST_MPL_AUX_NA_PARAM(P)
    >
struct first
{
    typedef typename P::first type;
    BOOST_MPL_AUX_LAMBDA_SUPPORT(1,first,(P))
};

template<
      typename BOOST_MPL_AUX_NA_PARAM(P)
    >
struct second
{
    typedef typename P::second type;
    BOOST_MPL_AUX_LAMBDA_SUPPORT(1,second,(P))
};


BOOST_MPL_AUX_NA_SPEC_NO_ETI(2, pair)
BOOST_MPL_AUX_NA_SPEC(1, first)
BOOST_MPL_AUX_NA_SPEC(1, second)

}}

#endif // BOOST_MPL_PAIR_HPP_INCLUDED
