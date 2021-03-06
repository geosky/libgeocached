//
//  geolib.cpp
//  geocached
//
//  Created by Denny C. Dai on 2013-06-21.
//  Copyright (c) 2013 Denny C. Dai. All rights reserved.
//

#include <geolib.h>
#include <cstdlib>
#include <GeographicLib/Geodesic.hpp>
#include <geohash.h>

using namespace GeographicLib;
using namespace std;

namespace libgeocached {
    
    //distance to circle center within the circle radius 
    bool GCPointInCircle(GCLocation point, GCCircle cirle){
        double distance; //in meters
        Geodesic::WGS84.Inverse(point.latitude, point.longitude, cirle.center.latitude, cirle.center.longitude, distance);
        return distance <= cirle.radius;
    }

    /*! evaluate if a circle overlaps with a rect represented using geohash binary format 
     
     
     */
    bool GCCircleRectOverlap(GCCircle circle, GCGeoHashBinary lat, GCGeoHashBinary lng, int bits){
        
        //translate a binary representation of lat / lng into a rect box
        GCRect rect = GCRectMake(GCDegreeLatitudeMin, GCDegreeLatitudeMax, GCDegreeLongitudeMin, GCDegreeLongitudeMax);
        
        for(int i=bits-1;i>=0;i--){
            
            if( ((lat >> i) & 0x01) == 0x01){  //high lat
                rect.lat_south = (rect.lat_north + rect.lat_south) / 2.0f; 
            }
            else{    //low lat
                rect.lat_north = (rect.lat_north + rect.lat_south) / 2.0f; 
            }
            
            
            if( ((lng >> i) & 0x01) == 0x01){  //high lng
                rect.lng_west = (rect.lng_east + rect.lng_west)/2.0f;
            }
            else{   //low lng
                rect.lng_east = (rect.lng_east + rect.lng_west)/2.0f;
            }
            
        }
        
        return GCCircleRectOverlap(circle, rect);
    }
    
    bool GCCircleRectOverlap(GCCircle circle, GCGeoHash geohash)
    {
        GEOHASH_area* area = GEOHASH_decode(geohash.c_str());
        GCRect rect = GCRectMake(area->latitude.min, area->latitude.max, area->longitude.min, area->longitude.max);
        free(area);
        return GCCircleRectOverlap(circle, rect);
    }
    
    
    /**
       overlap is any of the following true 
     
       - circle centre within rect
       - circle cross any of the rect edge lines 
     
     **/
    bool GCCircleRectOverlap(GCCircle circle, GCRect rect)
    {
        return GCPointInRect(circle.center, rect) ||
                GCCircleCrossLatitude(circle, rect.lat_north) ||
                GCCircleCrossLatitude(circle, rect.lat_south) ||
                GCCircleCrossLongitude(circle, rect.lng_west) ||
                GCCircleCrossLongitude(circle, rect.lng_east);
    }
    
    bool GCCircleCrossLongitude(GCCircle circle, GCDegree longitude)
    {
        double heading = circle.center.longitude < longitude ? 90 : 270;
        double proj_lat, proj_lon;
        Geodesic::WGS84.Direct(circle.center.latitude, circle.center.longitude, heading, circle.radius, proj_lat, proj_lon);
        return (circle.center.longitude - longitude)*(proj_lon - longitude) < 0;
    }
    

    //follow a geodesic from circle center by radius distance, then
    //see if the end point passed the latitude 
    bool GCCircleCrossLatitude(GCCircle circle, GCDegree latitude)
    {
        double heading =  circle.center.latitude < latitude ? 0 : 180;
        double proj_lat, proj_lon;
        Geodesic::WGS84.Direct(circle.center.latitude, circle.center.longitude, heading, circle.radius, proj_lat, proj_lon);
        return (circle.center.latitude - latitude) * (proj_lat - latitude) < 0;
    }
    
    bool GCPointInRect(GCLocation point, GCRect rect){
        return (point.latitude <= rect.lat_north) &&
                (point.latitude >= rect.lat_south) &&
                (point.longitude >= rect.lng_west) &&
                (point.longitude <= rect.lng_east);
    }
    
    
    
}