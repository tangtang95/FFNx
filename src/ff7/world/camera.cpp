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

#include "camera.h"
#include "world.h"
#include "utils.h"

#include "defs.h"
#include "../../patch.h"

namespace ff7::world
{
    WorldCamera worldCamera;

    void update_world_camera_front(int current_key_input, int prev_key_input)
    {
        int world_map_type = *ff7_externals.world_map_type_E045E8;
        int camera_view_type = *ff7_externals.world_camera_viewtype_DFC4B4;
        int movement_multiplier = *ff7_externals.world_movement_multiplier_DFC480;

        bool highwind_view_and_square_pressed = camera_view_type == HIGHWIND_VIEW && is_key_pressed(current_key_input, SQUARE);
        if (world_map_type == SNOWSTORM)
        {
            int delta_camera_step = ff7_externals.world_snowstorm_get_camera_movement_758B12(current_key_input & ANY_DIRECTIONAL_KEY,
                                    !is_key_released(current_key_input, prev_key_input, CIRCLE));
            *ff7_externals.world_camera_front_DFC484 += delta_camera_step;
        }
        else if (!highwind_view_and_square_pressed)
        {
            int direction_key, camera_rotate_speed = 0;
            if (camera_view_type == FRONT_VIEW && world_map_type != UNDERWATER && is_key_pressed(current_key_input, DOWN))
                direction_key = RIGHT;
            else
                direction_key = LEFT;
            camera_rotate_speed = is_key_pressed(current_key_input, direction_key) + is_key_pressed(current_key_input, L1);
            if (camera_rotate_speed)
            {
                int divider = world_map_type == UNDERWATER && (current_key_input & L1) == 0;
                *ff7_externals.world_camera_front_DFC484 -= ((2 - is_key_pressed(current_key_input, UP | DOWN)) * camera_rotate_speed * 8 * movement_multiplier) >> divider;
            }

            if (camera_view_type == FRONT_VIEW && world_map_type != UNDERWATER && is_key_pressed(current_key_input, DOWN))
                direction_key = LEFT;
            else
                direction_key = RIGHT;
            camera_rotate_speed = is_key_pressed(current_key_input, direction_key) + is_key_pressed(current_key_input, R1);
            if (camera_rotate_speed)
            {
                int divider = world_map_type == UNDERWATER && (current_key_input & R1) == 0;
                *ff7_externals.world_camera_front_DFC484 += ((2 - is_key_pressed(current_key_input, UP | DOWN)) * camera_rotate_speed * 8 * movement_multiplier) >> divider;
            }

            // auto rotationSpeed = worldCamera.getRotationSpeed();
            // worldCamera.rotationOffset.x = std::min(std::max(worldCamera.rotationOffset.x - rotationSpeed.x, -175.0f), -95.0f);
            // worldCamera.rotationOffset.y = std::remainder(worldCamera.rotationOffset.y - rotationSpeed.y, 360.0f);

            // *ff7_externals.world_camera_front_DFC484 = worldCamera.rotationOffset.y * 4096 / 360.0f;
        }

        if (*ff7_externals.world_camera_front_DFC484 >= 0)
        {
            if (*ff7_externals.world_camera_front_DFC484 >= 4096)
                *ff7_externals.world_camera_front_DFC484 -= 4096;
        }
        else
            *ff7_externals.world_camera_front_DFC484 = 4096;
    }

    void update_world_camera_rotation_y()
    {
        int camera_view_type = *ff7_externals.world_camera_viewtype_DFC4B4;
        int movement_multiplier = *ff7_externals.world_movement_multiplier_DFC480;

        if (camera_view_type != HIGHWIND_VIEW)
            *ff7_externals.world_camera_var2_DE6B4C = (*ff7_externals.world_camera_var1_DF542C + 3 * (*ff7_externals.world_camera_var2_DE6B4C)) >> 2;

        if (*ff7_externals.world_camera_rotation_y_DFC474 >= *ff7_externals.world_camera_front_DFC484 - 2048)
        {
            if (*ff7_externals.world_camera_rotation_y_DFC474 > *ff7_externals.world_camera_front_DFC484 + 2048)
                *ff7_externals.world_camera_rotation_y_DFC474 -= 4096;
        }
        else
            *ff7_externals.world_camera_rotation_y_DFC474 += 4096;

        if (movement_multiplier == 1)
            *ff7_externals.world_camera_rotation_y_DFC474 = (*ff7_externals.world_camera_front_DFC484 + 31 * (*ff7_externals.world_camera_rotation_y_DFC474)) >> 5;
        else
            *ff7_externals.world_camera_rotation_y_DFC474 = (*ff7_externals.world_camera_front_DFC484 + 15 * (*ff7_externals.world_camera_rotation_y_DFC474)) >> 4;

        //*ff7_externals.world_camera_rotation_y_DFC474 = *ff7_externals.world_camera_front_DFC484;
    }

    void update_world_camera(short world_camera_rotation_y)
    {
        bx::Vec3 up = { 0, 1, 0 };
        bx::Vec3 right = { 1, 0, 0 };
        auto quaternionH = bx::fromAxisAngle(up, M_PI * worldCamera.rotationOffset.y / 180.0f);
        auto quaternionV = bx::fromAxisAngle(right, M_PI * worldCamera.rotationOffset.x / 180.0f);

        auto quaternion = bx::mul(quaternionV, quaternionH);
        quaternion = bx::normalize(quaternion);

        float rotMat[16];
        bx::mtxFromQuaternion(rotMat, quaternion);

        float candidateDist = 10000 - (worldCamera.zoomOffset + worldCamera.getZoomSpeed());
        if(candidateDist < worldCamera.maxZoomDist && candidateDist > worldCamera.minZoomDist)
        {
            worldCamera.zoomOffset = worldCamera.zoomOffset + worldCamera.getZoomSpeed();
        }

        auto rot_matrix = (struct rotation_matrix*)(0xDFC448);
        rot_matrix->r3_sub_matrix[0][0] = rotMat[0] * 4096;
        rot_matrix->r3_sub_matrix[0][1] = rotMat[1] * 4096;
        rot_matrix->r3_sub_matrix[0][2] = rotMat[2] * 4096;

        rot_matrix->r3_sub_matrix[1][0] = rotMat[4] * 4096;
        rot_matrix->r3_sub_matrix[1][1] = rotMat[5] * 4096;
        rot_matrix->r3_sub_matrix[1][2] = rotMat[6] * 4096;

        rot_matrix->r3_sub_matrix[2][0] = rotMat[8] * 4096;
        rot_matrix->r3_sub_matrix[2][1] = rotMat[9] * 4096;
        rot_matrix->r3_sub_matrix[2][2] = rotMat[10] * 4096;

        auto translation_matrix = (struct transform_matrix*)(0xDE6A20);
        translation_matrix->pos_x = 0;
        translation_matrix->pos_y = 0;
        translation_matrix->pos_z = 10000 - worldCamera.zoomOffset;
    }

    void WorldCamera::setRotationSpeed(float rotX, float rotY, float rotZ)
    {
        rotationSpeed.x = rotX / static_cast<float>(common_frame_multiplier);
        rotationSpeed.y = rotY / static_cast<float>(common_frame_multiplier);
        rotationSpeed.z = rotZ / static_cast<float>(common_frame_multiplier);
    }

    void WorldCamera::setZoomSpeed(float speed)
    {
        zoomSpeed = speed / static_cast<float>(common_frame_multiplier);
    }

    void WorldCamera::reset()
    {
        rotationOffset.x = 0.0f;
        rotationOffset.y = 0.0f;
        zoomOffset = 0.0f;
    }
}