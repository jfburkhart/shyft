#include "test_pch.h"
#include "core/utctime_utilities.h"
#include "core/geo_cell_data.h"
#include <armadillo>
#include <vector>
#include <chrono>
#include <boost/math/constants/constants.hpp>
#include "core/radiation.h"


namespace shyft::test {
    using shyft::core::geo_cell_data;
    struct cell {
        geo_cell_data geo;
    };
}

TEST_SUITE("radiation") {
    using shyft::core::radiation::parameter;
    using shyft::core::radiation::calculator;
    using shyft::core::radiation::surface_normal;
    using shyft::core::calendar;
    using shyft::test::cell;
    using std::vector;
    using shyft::core::utctime;
    // test basics: creation, etc

    TEST_CASE("check_hour_angle"){
        /** \brief Check Earth Hour Angle
         * based on ref.: Lawrence Dingman Physical Hydrology, Third Edition, 2015, p.574, no EOT correction provided*/
        parameter p;
        calculator r(p);
        calendar utc_cal;
        double lat = 56.0;
        utctime t1, t2, t3;
        arma::vec surface_normal({0.0,0.0,1.0});
        t1 = utc_cal.time(1970, 1, 1, 10, 30, 0, 0);
        t2 = utc_cal.time(1970, 6, 1, 12, 0, 0, 0);
        t3 = utc_cal.time(1970, 9, 1, 23, 30, 0, 0);
        // using simple formulae for checking, need to provide test for EOT
        r.rso_cs_radiation(lat, t1, surface_normal);
                FAST_CHECK_EQ(r.hra(), doctest::Approx(-22.5));// earth hour angle
        r.rso_cs_radiation(lat, t2, surface_normal);
                FAST_CHECK_EQ(r.hra(), doctest::Approx(0.0));// earth hour angle
        r.rso_cs_radiation(lat, t3, surface_normal);
                FAST_CHECK_EQ(r.hra(), doctest::Approx(172.5));// earth hour angle

    }
    TEST_CASE("check_declination_angle"){
        /** \brief Check Earth declination angle
         * based on ref.: Lawrence Dingman Physical Hydrology, Third Edition, 2015, p.574, eq.(D.5) */
        parameter p;
        calculator r(p);
        calendar utc_cal;
        double lat = 56.0;
        utctime t1, t2, t3, t4;
        arma::vec surface_normal({0.0,0.0,1.0});
        t1 = utc_cal.time(1970, 12, 21, 12, 30, 0, 0);
        t2 = utc_cal.time(1970, 6, 21, 12, 30, 0, 0);
        t3 = utc_cal.time(1970, 3, 21, 12, 30, 0, 0);
        t4 = utc_cal.time(1970, 9, 24, 12, 30, 0, 0);
        r.rso_cs_radiation(lat, t1, surface_normal);
                FAST_CHECK_EQ(r.declination(), doctest::Approx(-23.5).epsilon(0.1));// earth declination angle 21/22 December (min)
        r.rso_cs_radiation(lat, t2, surface_normal);
                FAST_CHECK_EQ(r.declination(), doctest::Approx(23.5).epsilon(0.1));// earth declination angle 21/22 June (max)
        r.rso_cs_radiation(lat, t3, surface_normal);
                FAST_CHECK_EQ(r.declination(), doctest::Approx(0.0).epsilon(0.1));// earth declination angle 20/21 March 0
        r.rso_cs_radiation(lat, t4, surface_normal);
                FAST_CHECK_EQ(r.declination(), doctest::Approx(0.0).epsilon(0.5));// earth declination angle 22/23 September 0 this gives a bit higher error
    }
    TEST_CASE("check_atm_pressures"){
        parameter p;
        calculator r(p);
        calendar utc_cal;
        double lat = 56.0;
        utctime t;
        arma::vec surface_normal({0.0,0.0,1.0});
        t = utc_cal.time(1970, 12, 21, 12, 30, 0, 0);
        r.rso_cs_radiation(lat, t, surface_normal, 0.0, 0.0, 0.0);
                FAST_CHECK_EQ(r.patm(), doctest::Approx(101.325).epsilon(0.1));
        r.rso_cs_radiation(lat, t, surface_normal, 0.0, 0.0, 1000.0);
                FAST_CHECK_EQ(r.patm(), doctest::Approx(89.88).epsilon(0.1)); // https://onlinelibrary.wiley.com/doi/pdf/10.1002/9781118568101.app2
        r.rso_cs_radiation(lat, t, surface_normal, 0.0, 0.0,  -1000.0);
                FAST_CHECK_EQ(r.patm(), doctest::Approx(113.93).epsilon(0.1));

    }
    TEST_CASE("check_vap_pressure"){
        /**\brief check saturation pressure (rhumidity=100)
         * based ref.:  Lawrence Dingman Physical Hydrology, Third Edition, 2015, p.113, fig.3.1*/
        parameter p;
        calculator r(p);
        calendar utc_cal;
        double lat = 56.0;
        utctime t;
        arma::vec surface_normal({0.0,0.0,1.0});
        t = utc_cal.time(1970, 12, 21, 12, 30, 0, 0);
        r.rso_cs_radiation(lat, t, surface_normal, 21.0, 100.0, 0.0);
                FAST_CHECK_EQ(r.ea(), doctest::Approx(2.5).epsilon(0.01));
        r.rso_cs_radiation(lat, t, surface_normal, 21.0, 50.0, 0.0);
                FAST_CHECK_EQ(r.ea(), doctest::Approx(1.25).epsilon(0.01));
        r.rso_cs_radiation(lat, t, surface_normal, 0.0, 100.0, 0.0);
                FAST_CHECK_EQ(r.ea(), doctest::Approx(0.6).epsilon(0.01));
        r.rso_cs_radiation(lat, t, surface_normal, 27.0, 100.0, 0.0);
                FAST_CHECK_EQ(r.ea(), doctest::Approx(3.6).epsilon(0.01));
        r.rso_cs_radiation(lat, t, surface_normal, -31.0, 100.0, 0.0);
                FAST_CHECK_EQ(r.ea(), doctest::Approx(0.03).epsilon(0.01));
    }
    TEST_CASE("check_slope_aspect"){
        /**\brief check slope and aspect*/
        parameter p;
        calculator r(p);
        calendar utc_cal;
        double lat = 56.0;
        utctime t;
        arma::vec surface_normal({0.0,0.0,1.0});
        t = utc_cal.time(1970, 12, 21, 12, 30, 0, 0);
        r.rso_cs_radiation(lat, t, surface_normal, -31.0, 100.0, 0.0);
                FAST_CHECK_EQ(r.slope(), doctest::Approx(0.0).epsilon(0.01));
                FAST_CHECK_EQ(r.aspect(), doctest::Approx(0.0).epsilon(0.01));
    }

    TEST_CASE("check_solar_radiation_horizontal"){
        parameter p;
        p.albedo = 0.2;
        p.turbidity = 1.0;
        calculator r(p);
        calendar utc_cal;
        double lat = 44.0;
        utctime t;
        // checking for horizontal surface Eugene, OR, p.64, fig.1b
        arma::vec surface_normal({0.0,0.0,1.0});
        double ra_sum = 0.0;
        double rso_sum = 0.0;
        double rs_sum = 0.0;
//        unsigned long doy_fig1[8] = {1,   50, 100, 150, 200, 250, 300, 350};
//        double ra_fig1[8] = {140, 250, 390, 490, 480, 370, 200, 120};
//        double rso_fi1[8] = {90,  140, 280, 350, 340, 250, 150, 90};
//        shyft::core::YMDhms ymdhms = utc_cal.from_day_number(doy_fig1[1]);
        for (int hour = 0;hour<24;++hour){
            t = utc_cal.time(1970, 06, 21, hour, 30, 0, 0); // June
            r.rso_cs_radiation(lat, t, surface_normal, 20.0, 50.0, 150.0);
            ra_sum += r.ra_radiation();
            rso_sum += r.rso_radiation();
            rs_sum += r.rs_radiation(260.0);
            std::cout<<"ra24: "<<r.ra24_<<std::endl;
            std::cout<<"sunset and rise: "<<r.sun_rise()<<"  "<<r.sun_set()<<std::endl;  
            std::cout<<"======================"<<std::endl;
        }
            std::cout<<"rs: "<<rs_sum/24<<std::endl;
            FAST_CHECK_EQ(ra_sum/24, doctest::Approx(490.0).epsilon(0.05));
            FAST_CHECK_EQ(rso_sum/24, doctest::Approx(350.0).epsilon(0.05));
        ra_sum = 0.0;
        rso_sum = 0.0;
        for (int hour = 0;hour<24;++hour){
            t = utc_cal.time(1970, 01, 1, hour, 30, 0, 0); // January
            r.rso_cs_radiation(lat, t, surface_normal, -21.0, 50.0, 150.0);
            ra_sum += r.ra_radiation();
            rso_sum += r.rso_radiation();
            std::cout<<"ra24: "<<r.ra24_<<std::endl;
            std::cout<<"sunset and rise: "<<r.sun_rise()<<"  "<<r.sun_set()<<std::endl;
            std::cout<<"======================"<<std::endl;
        }
                FAST_CHECK_EQ(ra_sum/24, doctest::Approx(130.0).epsilon(0.05));
                FAST_CHECK_EQ(rso_sum/24, doctest::Approx(85.0).epsilon(0.05));
        ra_sum = 0.0;
        rso_sum = 0.0;
        for (int hour = 0;hour<24;++hour){
            t = utc_cal.time(1970, 12, 12, hour, 30, 0, 0); // December
            r.rso_cs_radiation(lat, t, surface_normal, -21.0, 50.0, 150.0);
            ra_sum += r.ra_radiation();
            rso_sum += r.rso_radiation();
            std::cout<<"ra24: "<<r.ra24_<<std::endl;                                    
            std::cout<<"sunset and rise: "<<r.sun_rise()<<"  "<<r.sun_set()<<std::endl; 
            std::cout<<"======================"<<std::endl;                             
        }
                FAST_CHECK_EQ(ra_sum/24, doctest::Approx(130.0).epsilon(0.05));
                FAST_CHECK_EQ(rso_sum/24, doctest::Approx(85.0).epsilon(0.05));

    }
    TEST_CASE("check_solar_radiation_slope_45s"){
        parameter p;
        p.albedo = 0.2;
        p.turbidity = 1.0;
        calculator r(p);
        calendar utc_cal;
        double lat = 44.0;
        utctime t;
        // checking for horizontal surface Eugene, OR, p.64, fig.1d
        // 24h  average radiation
        double slope = 45*shyft::core::pi/180; // 45 S
        double proj = sin(slope);
        //double aspect = 45*shyft::core::pi/180;// facing south
        double aspect = 0.0*shyft::core::pi/180;// facing south
        arma::vec surface_normal({proj*cos(aspect),proj*sin(aspect),cos(slope)});
        double ra_sum = 0.0;
        double rahor_sum = 0.0;
        double rso_sum = 0.0;
        for (int hour = 0;hour<23;++hour){
            t = utc_cal.time(1970, 06, 21, hour, 30, 0, 0); // June
            r.rso_cs_radiation(lat, t, surface_normal, 20.0, 50.0, 150.0);
            ra_sum += r.ra_radiation();
            rahor_sum += r.ra_radiation_hor();
            rso_sum += r.rso_radiation();
            std::cout<<"ra24: "<<r.ra24_<<std::endl;
            std::cout<<"sunset and rise: "<<r.sun_rise()<<"  "<<r.sun_set()<<std::endl;  
            std::cout<<"======================"<<std::endl;
        }
                FAST_CHECK_EQ(ra_sum/24, doctest::Approx(375.0).epsilon(0.05));
                FAST_CHECK_EQ(rso_sum/24, doctest::Approx(290.0).epsilon(0.05));
        ra_sum = 0.0;
        rso_sum = 0.0;
        for (int hour = 0;hour<24;++hour){
            t = utc_cal.time(1970, 01, 1, hour, 30, 0, 0); // January
            r.rso_cs_radiation(lat, t, surface_normal, -21.0, 50.0, 150.0);
            ra_sum += r.ra_radiation();
            rso_sum += r.rso_radiation();
        }
                FAST_CHECK_EQ(ra_sum/24, doctest::Approx(390.0).epsilon(0.05));
                FAST_CHECK_EQ(rso_sum/24, doctest::Approx(200.0).epsilon(0.05));
        ra_sum = 0.0;
        rso_sum = 0.0;
        for (int hour = 0;hour<24;++hour){
            t = utc_cal.time(1970, 12, 21, hour, 30, 0, 0); // December
            r.rso_cs_radiation(lat, t, surface_normal, -21.0, 50.0, 150.0);
            ra_sum += r.ra_radiation();
            rso_sum += r.rso_radiation();
        }
                FAST_CHECK_EQ(ra_sum/24, doctest::Approx(390.0).epsilon(0.05));
                FAST_CHECK_EQ(rso_sum/24, doctest::Approx(200.0).epsilon(0.05));

    }
    TEST_CASE("check_solar_radiation_slope_90S"){
        parameter p;
        p.albedo = 0.05;
        p.turbidity = 1.0;
        calculator r(p);
        calendar utc_cal;
        double lat = 44.0;
        utctime t;
        // checking for horizontal surface Eugene, OR, p.64, fig.1e
        double slope = 90*shyft::core::pi/180; // 90 S
        double proj = sin(slope);
        double aspect = 0.0;// facing south
        arma::vec surface_normal({proj*cos(aspect),proj*sin(aspect),cos(slope)});
        double ra_sum = 0.0;
        double rso_sum = 0.0;
        for (int hour = 0;hour<24;++hour){
            t = utc_cal.time(1970, 06, 21, hour, 30, 0, 0); // June
            r.rso_cs_radiation(lat, t, surface_normal, 20.0, 50.0, 150.0);
            ra_sum += r.ra_radiation();
            rso_sum += r.rso_radiation();
        }
                FAST_CHECK_EQ(ra_sum/24, doctest::Approx(100.0).epsilon(0.05));
                FAST_CHECK_EQ(rso_sum/24, doctest::Approx(90.0).epsilon(0.05));
        ra_sum = 0.0;
        rso_sum = 0.0;
        for (int hour = 0;hour<24;++hour){
            t = utc_cal.time(1970, 01, 1, hour, 30, 0, 0); // January
            r.rso_cs_radiation(lat, t, surface_normal, -21.0, 50.0, 150.0);
            ra_sum += r.ra_radiation();
            rso_sum += r.rso_radiation();
        }
                FAST_CHECK_EQ(ra_sum/24, doctest::Approx(410.0).epsilon(0.05));
                FAST_CHECK_EQ(rso_sum/24, doctest::Approx(200.0).epsilon(0.05));
        ra_sum = 0.0;
        rso_sum = 0.0;
        for (int hour = 0;hour<24;++hour){
            t = utc_cal.time(1970, 12, 21, hour, 30, 0, 0); // December
            r.rso_cs_radiation(lat, t, surface_normal, -21.0, 50.0, 150.0);
            ra_sum += r.ra_radiation();
            rso_sum += r.rso_radiation();
        }
                FAST_CHECK_EQ(ra_sum/24, doctest::Approx(410.0).epsilon(0.05));
                FAST_CHECK_EQ(rso_sum/24, doctest::Approx(200.0).epsilon(0.05));

    }
    TEST_CASE("check_solar_radiation_slope_90N"){
        parameter p;
        p.albedo = 0.05;
        p.turbidity = 1.0;
        calculator r(p);
        calendar utc_cal;
        double lat = 44.0;
        utctime t;
        // checking for horizontal surface Eugene, OR, p.64, fig.1f
        double slope = 90*shyft::core::pi/180; // 90 N
        double proj = sin(slope);
        double aspect = 180*shyft::core::pi/180;// facing north
        arma::vec surface_normal({proj*cos(aspect),proj*sin(aspect),cos(slope)});
        double ra_sum = 0.0;
        double rso_sum = 0.0;
        double rahor_sum = 0.0;
        for (int hour = 0;hour<24;++hour){
            t = utc_cal.time(1970, 06, 21, hour, 30, 0, 0); // June
            r.rso_cs_radiation(lat, t, surface_normal, 20.0, 50.0, 150.0);
            ra_sum += r.ra_radiation();
            std::cout<<"ra: "<<r.ra_radiation()<<std::endl;
            rso_sum += r.rso_radiation();
            std::cout<<"rso: "<<r.rso_radiation()<<std::endl;
            rahor_sum += r.ra_radiation_hor();
            std::cout<<"rahor: "<<r.ra_radiation_hor()<<std::endl;
            std::cout<<"ra24: "<<r.ra24_<<std::endl;
            std::cout<<"sunset and rise: "<<r.sun_rise()<<"  "<<r.sun_set()<<std::endl;
            std::cout<<"======================"<<std::endl;
        }
                FAST_CHECK_EQ(ra_sum/24, doctest::Approx(100.0).epsilon(0.05));
                FAST_CHECK_EQ(rso_sum/24, doctest::Approx(50.0).epsilon(0.05));

        ra_sum = 0.0;
        rso_sum = 0.0;
        rahor_sum = 0.0;
        for (int hour = 0;hour<24;++hour){
            t = utc_cal.time(1970, 01, 1, hour, 30, 0, 0); // January
            r.rso_cs_radiation(lat, t, surface_normal, -21.0, 50.0, 150.0);
            ra_sum += r.ra_radiation();
            rso_sum += r.rso_radiation();
            rahor_sum += r.ra_radiation_hor();
        }
                FAST_CHECK_EQ(ra_sum/24, doctest::Approx(0.0).epsilon(0.05));
                FAST_CHECK_EQ(rahor_sum/24, doctest::Approx(50.0).epsilon(0.05));
                FAST_CHECK_EQ(rso_sum/24, doctest::Approx(10.0).epsilon(0.05));
                FAST_CHECK_EQ(r.sun_rise(), doctest::Approx(10.0).epsilon(0.01));
                FAST_CHECK_EQ(r.sun_set(), doctest::Approx(23.0).epsilon(0.01)); 
        ra_sum = 0.0;
        rso_sum = 0.0;
        for (int hour = 0;hour<24;++hour){
            t = utc_cal.time(1970, 12, 21, hour, 30, 0, 0); // December
            r.rso_cs_radiation(lat, t, surface_normal, -21.0, 50.0, 150.0);
            ra_sum += r.ra_radiation();
            rso_sum += r.rso_radiation();
        }
                FAST_CHECK_EQ(ra_sum/24, doctest::Approx(0.0).epsilon(0.05));
                FAST_CHECK_EQ(rso_sum/24, doctest::Approx(10.0).epsilon(0.05));

    }   
    TEST_CASE("surface_normal_from_cells") {
        vector<cell> cells;
        auto r= surface_normal(cells);
    }
}
