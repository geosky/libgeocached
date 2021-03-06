//
//  main.cpp
//  geocached
//
//  Created by Denny C. Dai on 2013-06-14.
//  Copyright (c) 2013 Denny C. Dai. All rights reserved.
//

#include <iostream>
#include <gtest/gtest.h>
#include <geohash.h>
#include <uuid/uuid.h>

#include <cell.hpp>
#include <matrix.hpp>

#include <gctree.hpp>
#include <geolib.h>

using namespace std;
using namespace libgeocached;


class DataObject{
public:
    
    DataObject(){
    }
    
    DataObject(string d){
        data = d;
    }
    string data;
    
    
};

TEST(GeoHashBinary, test){
    
    GCCircle circle = GCCircleMake(GCLocationMake(23.23234, -123.34324), 1000);
    
    int bits = 15;
    long lat = 0b101000010000101;
    long lng = 0b001010000100101;
    
    
    EXPECT_TRUE( GCCircleRectOverlap(circle, lat, lng, bits) );
}

//query for objects within a circular region
TEST(CircleQuery, test){
    
    Matrix<DataObject> matrix;

    //data insertion
    ObjectID id1 = ObjectIDNew();
    EXPECT_TRUE(matrix.insert(id1, DataObject("hello"), GCLocationMake(23.23234, -123.34324)));
    
    
    std::vector<DataObject> objs;
    matrix.objs_in_circle(GCCircleMake(GCLocationMake(23.23234, -123.34324), 1000), objs);
    
    EXPECT_TRUE(objs.size()==1);
    
    for(DataObject& obj : objs)
        cout << obj.data << endl;
    
}

TEST(GEOLIB, test){
    
    GCLocation loc = GCLocationMake(12.23231, 88.1232);
    GCRect rect = GCRectMake(-20, 20, 40, 100);
    GCCircle circle = GCCircleMake( loc, 8000 * 1000);
    
    EXPECT_TRUE( GCPointInRect(loc,rect) );
    EXPECT_TRUE( GCCircleCrossLatitude(circle, 12));
    EXPECT_TRUE( GCCircleCrossLongitude(circle, 88));
    
    
    EXPECT_TRUE( GCCircleRectOverlap(circle, rect) );
    
    GCCircle circle2 = GCCircleMake(GCLocationMake(22, 80), 8000*1000);
    EXPECT_TRUE( GCCircleRectOverlap(circle2, rect));

    
    GCCircle circle3 = GCCircleMake(GCLocationMake(45, 80), 80*1000);
    EXPECT_TRUE(! GCCircleRectOverlap(circle3, rect));
    
    
    EXPECT_TRUE(GCPointInCircle(GCLocationMake(12.23123, 88.123434),
                GCCircleMake(GCLocationMake(12.23123, 88.123434), 1000)));
    
    EXPECT_FALSE(GCPointInCircle(GCLocationMake(-12.23123, -88.123434),
                GCCircleMake(GCLocationMake(12.23123, 88.123434), 1000)) );
    
    
    cout << GCCircleRectOverlap(circle, "ezs4")  << endl;
}


TEST(GCTree, test){
    
    GCTree tree;
    EXPECT_TRUE(tree.insert("ezs4"));
    EXPECT_TRUE(tree.insert("cbcb"));

    EXPECT_TRUE(tree.exists("ezs4"));
    EXPECT_TRUE(tree.exists("cbcb"));
    
    tree.traverse(10, [](GCGeoHash hash){
        cout << "HASH " << hash << endl;
    });

    tree.traverse(5, [](GCGeoHash hash){
        cout << "HASH " << hash << endl;
    });
    
    EXPECT_TRUE( tree.remove("ezs4"));
    EXPECT_TRUE(!tree.remove("zzzz"));

    EXPECT_TRUE( tree.remove("cbcb"));
    EXPECT_TRUE(!tree.remove("cbcb"));
    
    EXPECT_TRUE(tree.empty());
}


TEST(UUIDTest, test){
    uuid_t myid;
    uuid_generate(myid);
    uuid_string_t myidStr;
    uuid_unparse(myid, myidStr);
    cout << "uuid is " << myidStr << endl;
}

TEST(Cell, test){
    
    Matrix<DataObject> matrix;
    
    
    //data insertion
    ObjectID id1 = ObjectIDNew();
    EXPECT_TRUE(matrix.insert(id1, DataObject("hello"), GCLocationMake(23.23234, -123.34324)));
    
    ObjectID id2 = ObjectIDNew();
    EXPECT_TRUE(matrix.insert(id2, DataObject("world"), GCLocationMake(34.1232, -23.34324)));
    
    ObjectID id3 = ObjectIDNew();
    EXPECT_TRUE(matrix.insert(id3, DataObject("denny"), GCLocationMake(77.2323, 123.34324)));

    //retrieval
    DataObject obj;
    EXPECT_TRUE(matrix.retrieve(id1, obj));
    EXPECT_TRUE(matrix.retrieve(id2, obj));
    EXPECT_TRUE(matrix.retrieve(id3, obj));

    //trversal
    matrix.traverse([](string key, DataObject& data){
       cout << "key - " << key <<  ",  data - " << data.data << endl;
    });


    //location upate
    EXPECT_TRUE(matrix.update_location(id1, {11.232323, 90.1312})); 
    EXPECT_TRUE(matrix.update_location(id2, {11.232323, 90.1312}));
    EXPECT_TRUE(matrix.update_location(id3, {11.232323, 90.1312}));

    matrix.traverse([](string key, DataObject& data){
        cout << "key - " << key <<  ",  data - " << data.data << endl;
    });
    
    
    //removal
    EXPECT_TRUE(matrix.remove(id1)); EXPECT_TRUE(matrix.size()==2);
    EXPECT_TRUE(matrix.remove(id2)); EXPECT_TRUE(matrix.size()==1);
    EXPECT_TRUE(matrix.remove(id3)); EXPECT_TRUE(matrix.size()==0);

    matrix.traverse([](string key, DataObject& data){
        cout << "key - " << key <<  ",  data - " << data.data << endl;
    });
    
    
}


TEST(GeoHash, test){
    char *hash = GEOHASH_encode(36.232, -123.324, 8);
    cout << "geohash: " << hash << endl;
    free(hash);
    
    {
        char *hash = GEOHASH_encode(36.232, -123.324, 7);
        cout << "geohash: " << hash << endl;
        free(hash);
        
    }
    
    
}

int main(int argc, const char * argv[])
{
    #ifdef __cplusplus
    cout << "cplusplus defined!" << endl;
    #endif
    
    ::testing::InitGoogleTest(&argc, (char**)argv);
    return RUN_ALL_TESTS();
}

