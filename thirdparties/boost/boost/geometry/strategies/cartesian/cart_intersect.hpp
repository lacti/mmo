// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2007-2012 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2013 Adam Wulkiewicz, Lodz, Poland.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_STRATEGIES_CARTESIAN_INTERSECTION_HPP
#define BOOST_GEOMETRY_STRATEGIES_CARTESIAN_INTERSECTION_HPP

#include <algorithm>

#include <boost/geometry/core/exception.hpp>

#include <boost/geometry/geometries/concepts/point_concept.hpp>
#include <boost/geometry/geometries/concepts/segment_concept.hpp>

#include <boost/geometry/arithmetic/determinant.hpp>
#include <boost/geometry/algorithms/detail/assign_values.hpp>

#include <boost/geometry/util/math.hpp>
#include <boost/geometry/util/select_calculation_type.hpp>

// Temporary / will be Strategy as template parameter
#include <boost/geometry/strategies/side.hpp>
#include <boost/geometry/strategies/cartesian/side_by_triangle.hpp>

#include <boost/geometry/strategies/side_info.hpp>


namespace boost { namespace geometry
{


namespace strategy { namespace intersection
{


#ifndef DOXYGEN_NO_DETAIL
namespace detail
{

template <std::size_t Dimension, typename Segment, typename T>
static inline void segment_arrange(Segment const& s, T& s_1, T& s_2, bool& swapped)
{
    s_1 = get<0, Dimension>(s);
    s_2 = get<1, Dimension>(s);
    if (s_1 > s_2)
    {
        std::swap(s_1, s_2);
        swapped = true;
    }
}

template <std::size_t Index, typename Segment>
inline typename geometry::point_type<Segment>::type get_from_index(
            Segment const& segment)
{
    typedef typename geometry::point_type<Segment>::type point_type;
    point_type point;
    geometry::detail::assign::assign_point_from_index
        <
            Segment, point_type, Index, 0, dimension<Segment>::type::value
        >::apply(segment, point);
    return point;
}

}
#endif

/***
template <typename T>
inline std::string rdebug(T const& value)
{
    if (math::equals(value, 0)) return "'0'";
    if (math::equals(value, 1)) return "'1'";
    if (value < 0) return "<0";
    if (value > 1) return ">1";
    return "<0..1>";
}
***/

/*!
    \see http://mathworld.wolfram.com/Line-LineIntersection.html
 */
template <typename Policy, typename CalculationType = void>
struct relate_cartesian_segments
{
    typedef typename Policy::return_type return_type;
    typedef typename Policy::segment_type1 segment_type1;
    typedef typename Policy::segment_type2 segment_type2;

    //typedef typename point_type<segment_type1>::type point_type;
    //BOOST_CONCEPT_ASSERT( (concept::Point<point_type>) );

    BOOST_CONCEPT_ASSERT( (concept::ConstSegment<segment_type1>) );
    BOOST_CONCEPT_ASSERT( (concept::ConstSegment<segment_type2>) );

    typedef typename select_calculation_type
        <segment_type1, segment_type2, CalculationType>::type coordinate_type;

#if defined(BOOST_GEOMETRY_DEBUG_ROBUSTNESS)
    static inline void debug_segments(std::string const& header, segment_type1 const& a, segment_type2 const& b)
    {
        std::cout << "Robustness issue: " << header << std::endl;
        std::cout 
            << "A: " << wkt(a) << std::endl
            << "B: " << wkt(b) << std::endl
            ;
    }
#endif

    // Relate segments a and b
    static inline return_type apply(segment_type1 const& a, segment_type2 const& b)
    {
        coordinate_type const dx_a = get<1, 0>(a) - get<0, 0>(a); // distance in x-dir
        coordinate_type const dx_b = get<1, 0>(b) - get<0, 0>(b);
        coordinate_type const dy_a = get<1, 1>(a) - get<0, 1>(a); // distance in y-dir
        coordinate_type const dy_b = get<1, 1>(b) - get<0, 1>(b);
        return apply(a, b, dx_a, dy_a, dx_b, dy_b);
    }

    // Relate segments a and b using precalculated differences.
    // This can save two or four subtractions in many cases
    static inline return_type apply(segment_type1 const& a, segment_type2 const& b,
            coordinate_type const& dx_a, coordinate_type const& dy_a,
            coordinate_type const& dx_b, coordinate_type const& dy_b)
    {
        typedef side::side_by_triangle<coordinate_type> side;

        coordinate_type const zero = 0;
        bool const a_is_point = math::equals(dx_a, zero) && math::equals(dy_a, zero);
        bool const b_is_point = math::equals(dx_b, zero) && math::equals(dy_b, zero);

        // Get sides of a relative to b, and b relative to a
        side_info sides;
        sides.set<0>
            (
                side::apply(detail::get_from_index<0>(b)
                    , detail::get_from_index<1>(b)
                    , detail::get_from_index<0>(a)),
                side::apply(detail::get_from_index<0>(b)
                    , detail::get_from_index<1>(b)
                    , detail::get_from_index<1>(a))
            );
        sides.set<1>
            (
                side::apply(detail::get_from_index<0>(a)
                    , detail::get_from_index<1>(a)
                    , detail::get_from_index<0>(b)),
                side::apply(detail::get_from_index<0>(a)
                    , detail::get_from_index<1>(a)
                    , detail::get_from_index<1>(b))
            );
        return apply(a, b, dx_a, dy_a, dx_b, dy_b, sides, a_is_point, b_is_point);
    }

    static inline return_type apply(segment_type1 const& a, segment_type2 const& b,
            side_info& sides, bool a_is_point, bool b_is_point)
    {
        coordinate_type const dx_a = get<1, 0>(a) - get<0, 0>(a); // distance in x-dir
        coordinate_type const dx_b = get<1, 0>(b) - get<0, 0>(b);
        coordinate_type const dy_a = get<1, 1>(a) - get<0, 1>(a); // distance in y-dir
        coordinate_type const dy_b = get<1, 1>(b) - get<0, 1>(b);
        return apply(a, b, dx_a, dy_a, dx_b, dy_b, sides, a_is_point, b_is_point);
    }

    static inline return_type apply(segment_type1 const& a, segment_type2 const& b,
            coordinate_type const& dx_a, coordinate_type const& dy_a,
            coordinate_type const& dx_b, coordinate_type const& dy_b,
            side_info& sides, bool a_is_point, bool b_is_point)
    {
        if(a_is_point && b_is_point)
        {
            if(math::equals(get<1,0>(a), get<1,0>(b)) && math::equals(get<1,1>(a), get<1,1>(b)))
            {
                 Policy::degenerate(a, true);
            }
            else
            {
                return Policy::disjoint();                
            }
        }

        bool collinear = sides.collinear();

        if (sides.same<0>() || sides.same<1>())
        {
            // Both points are at same side of other segment, we can leave
            return Policy::disjoint();
        }

        // Degenerate cases: segments of single point, lying on other segment, are not disjoint
        if (a_is_point)
        {
            return Policy::degenerate(a, true);
        }
        if (b_is_point)
        {
            return Policy::degenerate(b, false);
        }

        typedef typename select_most_precise
            <
                coordinate_type, double
            >::type promoted_type;

        // r: ratio 0-1 where intersection divides A/B
        // (only calculated for non-collinear segments)
        promoted_type r;
        if (! collinear)
        {
            // Calculate determinants - Cramers rule
            coordinate_type const wx = get<0, 0>(a) - get<0, 0>(b);
            coordinate_type const wy = get<0, 1>(a) - get<0, 1>(b);
            promoted_type const d = geometry::detail::determinant<promoted_type>(dx_a, dy_a, dx_b, dy_b);
            promoted_type const da = geometry::detail::determinant<promoted_type>(dx_b, dy_b, wx, wy);

            coordinate_type const zero = coordinate_type();
            if (math::equals(d, zero))
            {
                // This is still a collinear case (because of FP imprecision this can occur here)
                // sides.debug();
                sides.set<0>(0,0);
                sides.set<1>(0,0);
                collinear = true;
            }
            else
            {
                r = da / d;

                if (! robustness_verify_r(a, b, r))
                {
                    return Policy::disjoint();
                }
            }
        }

        if(collinear)
        {
            // Use dimension 0 for collinear cases if differences in x exceeds differences in y
            if (math::abs(dx_a) + math::abs(dx_b) >= math::abs(dy_a) + math::abs(dy_b))
            {
                return relate_collinear<0>(a, b);
            }
            else
            {
                // Y direction contains larger segments (maybe dx is zero)
                return relate_collinear<1>(a, b);
            }
        }

        return Policy::segments_intersect(sides, r,
            dx_a, dy_a, dx_b, dy_b,
            a, b);
    }

private :


    // Ratio should lie between 0 and 1
    // Also these three conditions might be of FP imprecision, the segments were actually (nearly) collinear
    template <typename T>
    static inline bool robustness_verify_r(
                segment_type1 const& a, segment_type2 const& b,
                T& r)
    {
        T const zero = 0;
        T const one = 1;
        if (r < zero || r > one)
        {
            if (verify_disjoint<0>(a, b) || verify_disjoint<1>(a, b))
            {
                // Can still be disjoint (even if not one is left or right from another)
                // This is e.g. in case #snake4 of buffer test.
                return false;
            }

            //std::cout << "ROBUSTNESS: correction of r " << r << std::endl;
            // sides.debug();

            // ROBUSTNESS: the r value can in epsilon-cases much larger than 1, while (with perfect arithmetic)
            // it should be one. It can be 1.14 or even 1.98049 or 2 (while still intersecting)

            // If segments are crossing (we can see that with the sides)
            // and one is inside the other, there must be an intersection point.
            // We correct for that.
            // This is (only) in case #ggl_list_20110820_christophe in unit tests

            // If segments are touching (two sides zero), of course they should intersect
            // This is (only) in case #buffer_rt_i in the unit tests)

            // If one touches in the middle, they also should intersect (#buffer_rt_j)

            // Note that even for ttmath r is occasionally > 1, e.g. 1.0000000000000000000000036191231203575
#if defined(BOOST_GEOMETRY_DEBUG_ROBUSTNESS)
            debug_segments("correcting r", a, b);
            std::cout << " --> r=" << r;
            if (r > 1.00000000000001 || r < -0.00000000000001)
            {
                std::cout << " !!!";
            }
            std::cout << std::endl << std::endl;
#endif

            if (r > one)
            {
                r = one;
            }
            else if (r < zero)
            {
                r = zero;
            }
        }
        return true;
    }

    template <std::size_t Dimension>
    static inline bool verify_disjoint(segment_type1 const& a,
                    segment_type2 const& b)
    {
        coordinate_type a_1, a_2, b_1, b_2;
        bool a_swapped = false, b_swapped = false;
        detail::segment_arrange<Dimension>(a, a_1, a_2, a_swapped);
        detail::segment_arrange<Dimension>(b, b_1, b_2, b_swapped);
        return math::smaller(a_2, b_1) || math::larger(a_1, b_2);
    }

    template <std::size_t Dimension>
    static inline return_type relate_collinear(segment_type1 const& a,
                                               segment_type2 const& b)
    {
        coordinate_type a_1, a_2, b_1, b_2;
        bool a_swapped = false, b_swapped = false;
        detail::segment_arrange<Dimension>(a, a_1, a_2, a_swapped);
        detail::segment_arrange<Dimension>(b, b_1, b_2, b_swapped);

        // TODO: these should be passed as integer too -> normal comparisons possible
        if (math::smaller(a_2, b_1) || math::larger(a_1, b_2))
        //if (a_2 < b_1 || a_1 > b_2)
        {
            return Policy::disjoint();
        }
        return relate_collinear(a, b, a_1, a_2, b_1, b_2, a_swapped, b_swapped);
    }

    /// Relate segments known collinear
    static inline return_type relate_collinear(segment_type1 const& a
            , segment_type2 const& b
            , coordinate_type a_1, coordinate_type a_2
            , coordinate_type b_1, coordinate_type b_2
            , bool a_swapped, bool b_swapped)
    {
        // All ca. 150 lines are about collinear rays
        // The intersections, if any, are always boundary points of the segments. No need to calculate anything.
        // However we want to find out HOW they intersect, there are many cases.
        // Most sources only provide the intersection (above) or that there is a collinearity (but not the points)
        // or some spare sources give the intersection points (calculated) but not how they align.
        // This source tries to give everything and still be efficient.
        // It is therefore (and because of the extensive clarification comments) rather long...

        // \see http://mpa.itc.it/radim/g50history/CMP/4.2.1-CERL-beta-libes/file475.txt
        // \see http://docs.codehaus.org/display/GEOTDOC/Point+Set+Theory+and+the+DE-9IM+Matrix
        // \see http://mathworld.wolfram.com/Line-LineIntersection.html

        // Because of collinearity the case is now one-dimensional and can be checked using intervals
        // This function is called either horizontally or vertically
        // We get then two intervals:
        // a_1-------------a_2 where a_1 < a_2
        // b_1-------------b_2 where b_1 < b_2
        // In all figures below a_1/a_2 denotes arranged intervals, a1-a2 or a2-a1 are still unarranged

        // Handle "equal", in polygon neighbourhood comparisons a common case

        bool const opposite = a_swapped ^ b_swapped;
        bool const both_swapped = a_swapped && b_swapped;

        // Check if segments are equal or opposite equal...
        bool const swapped_a1_eq_b1 = math::equals(a_1, b_1);
        bool const swapped_a2_eq_b2 = math::equals(a_2, b_2);

        if (swapped_a1_eq_b1 && swapped_a2_eq_b2)
        {
            return Policy::segment_equal(a, opposite);
        }

        bool const swapped_a2_eq_b1 = math::equals(a_2, b_1);
        bool const swapped_a1_eq_b2 = math::equals(a_1, b_2);

        bool const a1_eq_b1 = both_swapped ? swapped_a2_eq_b2 : a_swapped ? swapped_a2_eq_b1 : b_swapped ? swapped_a1_eq_b2 : swapped_a1_eq_b1;
        bool const a2_eq_b2 = both_swapped ? swapped_a1_eq_b1 : a_swapped ? swapped_a1_eq_b2 : b_swapped ? swapped_a2_eq_b1 : swapped_a2_eq_b2;

        bool const a1_eq_b2 = both_swapped ? swapped_a2_eq_b1 : a_swapped ? swapped_a2_eq_b2 : b_swapped ? swapped_a1_eq_b1 : swapped_a1_eq_b2;
        bool const a2_eq_b1 = both_swapped ? swapped_a1_eq_b2 : a_swapped ? swapped_a1_eq_b1 : b_swapped ? swapped_a2_eq_b2 : swapped_a2_eq_b1;




        // The rest below will return one or two intersections.
        // The delegated class can decide which is the intersection point, or two, build the Intersection Matrix (IM)
        // For IM it is important to know which relates to which. So this information is given,
        // without performance penalties to intersection calculation

        bool const has_common_points = swapped_a1_eq_b1 || swapped_a1_eq_b2 || swapped_a2_eq_b1 || swapped_a2_eq_b2;


        // "Touch" -> one intersection point -> one but not two common points
        // -------->             A (or B)
        //         <----------   B (or A)
        //        a_2==b_1         (b_2==a_1 or a_2==b1)

        // The check a_2/b_1 is necessary because it excludes cases like
        // ------->
        //     --->
        // ... which are handled lateron

        // Corresponds to 4 cases, of which the equal points are determined above
        // #1: a1---->a2 b1--->b2   (a arrives at b's border)
        // #2: a2<----a1 b2<---b1   (b arrives at a's border)
        // #3: a1---->a2 b2<---b1   (both arrive at each others border)
        // #4: a2<----a1 b1--->b2   (no arrival at all)
        // Where the arranged forms have two forms:
        //    a_1-----a_2/b_1-------b_2 or reverse (B left of A)
        if ((swapped_a2_eq_b1 || swapped_a1_eq_b2) && ! swapped_a1_eq_b1 && ! swapped_a2_eq_b2)
        {
            if (a2_eq_b1) return Policy::collinear_touch(get<1, 0>(a), get<1, 1>(a), 0, -1);
            if (a1_eq_b2) return Policy::collinear_touch(get<0, 0>(a), get<0, 1>(a), -1, 0);
            if (a2_eq_b2) return Policy::collinear_touch(get<1, 0>(a), get<1, 1>(a), 0, 0);
            if (a1_eq_b1) return Policy::collinear_touch(get<0, 0>(a), get<0, 1>(a), -1, -1);
        }


        // "Touch/within" -> there are common points and also an intersection of interiors:
        // Corresponds to many cases:
        // #1a: a1------->a2  #1b:        a1-->a2
        //          b1--->b2         b1------->b2
        // #2a: a2<-------a1  #2b:        a2<--a1
        //          b1--->b2         b1------->b2
        // #3a: a1------->a2  #3b:        a1-->a2
        //          b2<---b1         b2<-------b1
        // #4a: a2<-------a1  #4b:        a2<--a1
        //          b2<---b1         b2<-------b1

        // Note: next cases are similar and handled by the code
        // #4c: a1--->a2
        //      b1-------->b2
        // #4d: a1-------->a2
        //      b1-->b2

        // For case 1-4: a_1 < (b_1 or b_2) < a_2, two intersections are equal to segment B
        // For case 5-8: b_1 < (a_1 or a_2) < b_2, two intersections are equal to segment A
        if (has_common_points)
        {
            // Either A is in B, or B is in A, or (in case of robustness/equals)
            // both are true, see below
            bool a_in_b = (b_1 < a_1 && a_1 < b_2) || (b_1 < a_2 && a_2 < b_2);
            bool b_in_a = (a_1 < b_1 && b_1 < a_2) || (a_1 < b_2 && b_2 < a_2);

            if (a_in_b && b_in_a)
            {
                // testcase "ggl_list_20110306_javier"
                // In robustness it can occur that a point of A is inside B AND a point of B is inside A,
                // still while has_common_points is true (so one point equals the other).
                // If that is the case we select on length.
                coordinate_type const length_a = geometry::math::abs(a_1 - a_2);
                coordinate_type const length_b = geometry::math::abs(b_1 - b_2);
                if (length_a > length_b)
                {
                    a_in_b = false;
                }
                else
                {
                    b_in_a = false;
                }
            }

            int const arrival_a = a_in_b ? 1 : -1;
            if (a2_eq_b2) return Policy::collinear_interior_boundary_intersect(a_in_b ? a : b, a_in_b, 0, 0, false);
            if (a1_eq_b2) return Policy::collinear_interior_boundary_intersect(a_in_b ? a : b, a_in_b, arrival_a, 0, true);
            if (a2_eq_b1) return Policy::collinear_interior_boundary_intersect(a_in_b ? a : b, a_in_b, 0, -arrival_a, true);
            if (a1_eq_b1) return Policy::collinear_interior_boundary_intersect(a_in_b ? a : b, a_in_b, arrival_a, -arrival_a, false);
        }



        // "Inside", a completely within b or b completely within a
        // 2 cases:
        // case 1:
        //        a_1---a_2        -> take A's points as intersection points
        //   b_1------------b_2
        // case 2:
        //   a_1------------a_2
        //       b_1---b_2         -> take B's points
        if (a_1 > b_1 && a_2 < b_2)
        {
            // A within B
            return Policy::collinear_a_in_b(a, opposite);
        }
        if (b_1 > a_1 && b_2 < a_2)
        {
            // B within A
            return Policy::collinear_b_in_a(b, opposite);
        }


        /*

        Now that all cases with equal,touch,inside,disjoint,
        degenerate are handled the only thing left is an overlap

        Either a1 is between b1,b2
        or a2 is between b1,b2 (a2 arrives)

        Next table gives an overview.
        The IP's are ordered following the line A1->A2

             |                                 |
             |          a_2 in between         |       a_1 in between
             |                                 |
        -----+---------------------------------+--------------------------
             |   a1--------->a2                |       a1--------->a2
             |          b1----->b2             |   b1----->b2
             |   (b1,a2), a arrives            |   (a1,b2), b arrives
             |                                 |
        -----+---------------------------------+--------------------------
        a sw.|   a2<---------a1*               |       a2<---------a1*
             |           b1----->b2            |   b1----->b2
             |   (a1,b1), no arrival           |   (b2,a2), a and b arrive
             |                                 |
        -----+---------------------------------+--------------------------
             |   a1--------->a2                |       a1--------->a2
        b sw.|           b2<-----b1            |   b2<-----b1
             |   (b2,a2), a and b arrive       |   (a1,b1), no arrival
             |                                 |
        -----+---------------------------------+--------------------------
        a sw.|    a2<---------a1*              |       a2<---------a1*
        b sw.|            b2<-----b1           |   b2<-----b1
             |   (a1,b2), b arrives            |   (b1,a2), a arrives
             |                                 |
        -----+---------------------------------+--------------------------
        * Note that a_1 < a_2, and a1 <> a_1; if a is swapped,
          the picture might seem wrong but it (supposed to be) is right.
        */

        if (b_1 < a_2 && a_2 < b_2)
        {
            // Left column, from bottom to top
            return
                both_swapped ? Policy::collinear_overlaps(get<0, 0>(a), get<0, 1>(a), get<1, 0>(b), get<1, 1>(b), -1,  1, opposite)
                : b_swapped  ? Policy::collinear_overlaps(get<1, 0>(b), get<1, 1>(b), get<1, 0>(a), get<1, 1>(a),  1,  1, opposite)
                : a_swapped  ? Policy::collinear_overlaps(get<0, 0>(a), get<0, 1>(a), get<0, 0>(b), get<0, 1>(b), -1, -1, opposite)
                :              Policy::collinear_overlaps(get<0, 0>(b), get<0, 1>(b), get<1, 0>(a), get<1, 1>(a),  1, -1, opposite)
                ;
        }
        if (b_1 < a_1 && a_1 < b_2)
        {
            // Right column, from bottom to top
            return
                both_swapped ? Policy::collinear_overlaps(get<0, 0>(b), get<0, 1>(b), get<1, 0>(a), get<1, 1>(a),  1, -1, opposite)
                : b_swapped  ? Policy::collinear_overlaps(get<0, 0>(a), get<0, 1>(a), get<0, 0>(b), get<0, 1>(b), -1, -1, opposite)
                : a_swapped  ? Policy::collinear_overlaps(get<1, 0>(b), get<1, 1>(b), get<1, 0>(a), get<1, 1>(a),  1,  1, opposite)
                :              Policy::collinear_overlaps(get<0, 0>(a), get<0, 1>(a), get<1, 0>(b), get<1, 1>(b), -1,  1, opposite)
                ;
        }
        // Nothing should goes through. If any we have made an error
#if defined(BOOST_GEOMETRY_DEBUG_ROBUSTNESS)
        debug_segments("unexpected behaviour", a, b);
#endif
        return Policy::error("Robustness issue, relate_cartesian_segments, unexpected behaviour");
    }
};


}} // namespace strategy::intersection

}} // namespace boost::geometry


#endif // BOOST_GEOMETRY_STRATEGIES_CARTESIAN_INTERSECTION_HPP
