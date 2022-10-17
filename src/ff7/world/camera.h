/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
//    Copyright (C) 2022 Cosmos                                             //
//    Copyright (C) 2022 Tang-Tang Zhou                                     //
//                                                                          //
//    This file is part of FFNx                                             //
//                                                                          //
//    FFNx is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    FFNx is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/
#pragma once

#include "../../common.h"

#include <bx/math.h>
#include <bx/bx.h>

namespace ff7::world
{
    enum input_key
    {
        L2 = 1,
        R2 = 2,
        L1 = 4,
        R1 = 8,
        TRIANGLE = 16,
        CIRCLE = 32,
        CROSS = 64,
        SQUARE = 128,
        SELECT = 256,
        UNK1 = 512,
        UNK2 = 1024,
        START = 2048,
        UP = 4096,
        RIGHT = 8192,
        DOWN = 16384,
        LEFT = 32768
    };

    enum map_type {
        OVERWORLD = 0,
        UNDERWATER = 2,
        SNOWSTORM = 3
    };

    enum world_camera_viewtype
    {
        TOP_DOWN_VIEW = 0,
        FRONT_VIEW = 2,
        UNKNOWN_VIEW = 3
    };

    class WorldCamera
    {
        public:
            WorldCamera() = default;
            ~WorldCamera() = default;

            void setRotationSpeed(float rotX, float rotY, float rotZ);
            vector3<float> getRotationSpeed();
            void setZoomSpeed(float speed);
            float getZoomSpeed();
            void reset();

            void controlCamera(vector3<short>* cameraPosition, vector3<short>* cameraFocusPosition);
            void controlWmCamera(bx::Vec3* cameraPosition, const bx::Vec3& cameraFocusPosition, float initialDist);
        public:
            vector3<float> rotationOffset = { -85, 0.0, 0.0 };
            float zoomOffset = 0.0f;
            const float minZoomDist = 2000.0f;
            const float maxZoomDist = 30000.0f;
        private:
            vector3<float> rotationSpeed = { 0.0, 0.0, 0.0 };
            float zoomSpeed = 0.0f;

            const float minVerticalAngle = 5.0f;
            const float maxVerticalAngle = 85.0f;
    };

    inline vector3<float> WorldCamera::getRotationSpeed()
    {
        return rotationSpeed;
    }

    inline float WorldCamera::getZoomSpeed()
    {
        return zoomSpeed;
    }

    extern WorldCamera worldCamera;
}