
#pragma once

#include "../ff7.h"
#include <cmath>

template<typename T>
T field_get_linear_interpolated_value(T initial_value, T final_value, int n_steps, int step_idx)
{
	return std::lerp(initial_value, final_value, step_idx / (float)n_steps);
}

template<typename T>
T field_get_smooth_interpolated_value(T initial_value, T final_value, int n_steps, int step_idx)
{
	float delta = final_value - initial_value;
	return initial_value + delta * (0.5f + sin(-(atan(1)*4)/2.f + (atan(1)*4) * (step_idx / (float)n_steps)) / 2.f);
}

template<typename T>
class FieldScrollingEngine
{
private:
    inline static constexpr float INVALID_VALUE = -1000000;
    inline static constexpr float MIN_STEP_INVERSE = 32.f;

    vector2<T> curr_delta_world_pos, initial_delta_world_pos, final_delta_world_pos;
    vector2<T> field_3d_world_pos;
    vector2<T> bg_main_layer_pos, bg_layer3_pos, bg_layer4_pos;

    void set_world_pos_based_on_player_pos(vector2<T>* point, int model_id);
    int sub_66307D(vector3<T> *position, vector2<T> *delta_position, int *param_3, int *param_4);
    int sub_64314F(vector3<T> *position, vector2<T> *bg_delta_position);
    void set_world_and_background_positions(vector2<T> delta_position, bool use_camdat_pan);
    void clip_with_camera_range(vector2<T>* point);
    void clip_layer3_camera_range(field_trigger_header *trigger_header, vector2<T> *delta_position);

public:
    void field_set_bg_values();
    void field_update_scripted_bg_movement();
    void ff7_field_update_background();
};