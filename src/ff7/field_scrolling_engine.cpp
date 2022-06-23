#include "field_scrolling_engine.h"
#include "../globals.h"
#include <functional>

template<typename T>
void FieldScrollingEngine<T>::clip_with_camera_range(vector2<T>* point)
{
    field_trigger_header* field_triggers_header_ptr = *ff7_externals.field_triggers_header;
    if (point->x > field_triggers_header_ptr->camera_range.right - 160)
        point->x = field_triggers_header_ptr->camera_range.right - 160;
    if (point->x < field_triggers_header_ptr->camera_range.left + 160)
        point->x = field_triggers_header_ptr->camera_range.left + 160;
    if (point->y > field_triggers_header_ptr->camera_range.top - 120)
        point->y = field_triggers_header_ptr->camera_range.top - 120;
    if (point->y < field_triggers_header_ptr->camera_range.bottom + 120)
        point->y = field_triggers_header_ptr->camera_range.bottom + 120;
}

template<typename T>
void FieldScrollingEngine<T>::clip_layer3_camera_range(field_trigger_header *trigger_header, vector2<T> *delta_position)
{
    if (trigger_header->field_14[0] == 1)
    {
        float diff_top_bottom = trigger_header->camera_range.top - 120 - (trigger_header->camera_range.bottom + 120);
        float diff_right_left = trigger_header->camera_range.right - 160 - (trigger_header->camera_range.left + 160);
        float temp_1 = -(diff_top_bottom * (trigger_header->camera_range.bottom + 120 - delta_position->y) + diff_right_left * (trigger_header->camera_range.left + 160 - delta_position->x));
        float temp_square_value = (diff_top_bottom * diff_top_bottom + diff_right_left * diff_right_left) / 256.f;
        delta_position->x = ((diff_right_left * temp_1 / temp_square_value) / 256.f) + trigger_header->camera_range.left + 160;
        delta_position->y = ((diff_top_bottom * temp_1 / temp_square_value) / 256.f) + trigger_header->camera_range.bottom + 120;
    }
    if (trigger_header->field_14[0] == 2)
    {
        float diff_bottom_top = trigger_header->camera_range.bottom + 120 - (trigger_header->camera_range.top - 120);
        float diff_right_left = trigger_header->camera_range.right - 160 - (trigger_header->camera_range.left + 160);
        float temp_1 = -((diff_bottom_top) * (trigger_header->camera_range.top - 120 - delta_position->y) + diff_right_left * (trigger_header->camera_range.left + 160 - delta_position->x));
        float temp_square_value = (diff_bottom_top * diff_bottom_top + diff_right_left * diff_right_left) / 256.f;
        delta_position->x = ((diff_right_left * temp_1 / temp_square_value) / 256.f) + trigger_header->camera_range.left + 160;
        delta_position->y = ((diff_bottom_top * temp_1 / temp_square_value) / 256.f) + trigger_header->camera_range.top - 120;
    }
}

template<typename T>
int FieldScrollingEngine<T>::sub_66307D(vector3<T> *position, vector2<T> *delta_position, int *param_3, int *param_4)
{
	int z_value;
	float matrix[16];
	vector3<float> temp_point, temp_point_1;
	vector3<float> position_copy;
    position_copy.x = position->x;
    position_copy.y = position->y;
    position_copy.z = position->z;
	byte* global_game_data = *(byte**)ff7_externals.global_game_data_90AAF0;

	ff7_externals.engine_sub_661465((short*)&global_game_data[16], matrix);
	ff7_externals.engine_sub_66CF7E(matrix, &position_copy, &temp_point);
	temp_point_1.x = (double)*(int*)&global_game_data[34] + temp_point.x;
	temp_point_1.y = (double)*(int*)&global_game_data[38] + temp_point.y;
	temp_point_1.z = (double)*(int*)&global_game_data[42] + temp_point.z;
	if (temp_point_1.z == 0.f)
	{
		z_value = 0;
	}
	else
	{
		delta_position->x = (temp_point_1.x * (*(float*)global_game_data) / temp_point_1.z + (*(float*)&global_game_data[56]));
		delta_position->y = (temp_point_1.y * (*(float*)global_game_data) / temp_point_1.z + (*(float*)&global_game_data[60]));
		z_value = (temp_point_1.z * 0.25f);
	}
	*param_3 = 0;
	*param_4 = 0;
	return z_value;
}

template<typename T>
int FieldScrollingEngine<T>::sub_64314F(vector3<T> *position, vector2<T> *bg_delta_position)
{
	int dummy_1, dummy_2;
	int camera_pos_z;

	ff7_externals.engine_sub_663673((uint16_t*)ff7_externals.field_camera_CFF3D8);
	ff7_externals.engine_sub_663707((uint32_t*)ff7_externals.field_camera_CFF3D8);
	ff7_externals.engine_sub_661976(ff7_externals.field_vector2_CFF204->x, ff7_externals.field_vector2_CFF204->y);
	camera_pos_z = this->sub_66307D<T>(position, bg_delta_position, &dummy_1, &dummy_2);
	ff7_externals.engine_sub_661976(ff7_externals.field_vector2_CFF1F4->x, ff7_externals.field_vector2_CFF1F4->y);
	return camera_pos_z;
}

template<typename T>
void FieldScrollingEngine<T>::set_world_pos_based_on_player_pos(vector2<T>* point, int model_id)
{
	vector3<T> player_position;
	field_event_data* field_event_data_ptr = *ff7_externals.field_event_data_ptr;

	player_position.x = field_event_data_ptr[model_id].model_pos.x / 4096.f;
	player_position.y = field_event_data_ptr[model_id].model_pos.y / 4096.f;
	player_position.z = ff7_externals.modules_global_object->field_16 + (field_event_data_ptr[model_id].model_pos.z / 4096.f);
	this->sub_64314F<T>(&player_position, point);
	point->x -= ff7_externals.field_vector2_CFF204->x;
	point->y -= ff7_externals.field_vector2_CFF204->y;
}

template<typename T>
void FieldScrollingEngine<T>::set_world_and_background_positions(vector2<T> delta_position, bool use_camdat_pan)
{
	field_trigger_header* field_triggers_header_ptr = *ff7_externals.field_triggers_header;
	int field_bg_multiplier = *ff7_externals.field_bg_multiplier;

	this->field_3d_world_pos.x = ff7_externals.modules_global_object->shake_bg_x.shake_curr_value + ff7_externals.field_bg_offset->x - delta_position.x - 160;
	this->field_3d_world_pos.x += (use_camdat_pan) ? -(*ff7_externals.field_camera_data)->pan_x : 0;
	this->field_3d_world_pos.x *= field_bg_multiplier;
	this->field_3d_world_pos.y = ff7_externals.modules_global_object->shake_bg_y.shake_curr_value + ff7_externals.field_bg_offset->y - delta_position.y - 120;
	this->field_3d_world_pos.y += (use_camdat_pan) ? (*ff7_externals.field_camera_data)->pan_y : 0;
	this->field_3d_world_pos.y *= field_bg_multiplier;
	this->bg_main_layer_pos.x = delta_position.x + 320 - ff7_externals.field_bg_offset->x - ff7_externals.modules_global_object->shake_bg_x.shake_curr_value;
	this->bg_main_layer_pos.y = delta_position.y + 232 - ff7_externals.field_bg_offset->y - ff7_externals.modules_global_object->shake_bg_y.shake_curr_value;
	this->bg_layer3_pos.x = (field_triggers_header_ptr->bg3_pos_x / 16.f) + ((field_triggers_header_ptr->bg3_speed_x * delta_position.x) / 256.f);
	this->bg_layer3_pos.x = remainder(this->bg_layer3_pos.x, field_triggers_header_ptr->bg3_width);
	this->bg_layer3_pos.x = this->bg_layer3_pos.x + 320 - ff7_externals.field_bg_offset->x - ff7_externals.modules_global_object->shake_bg_x.shake_curr_value;
	this->bg_layer3_pos.y = (field_triggers_header_ptr->bg3_pos_y / 16.f) + ((field_triggers_header_ptr->bg3_speed_y * delta_position.y) / 256.f);
	this->bg_layer3_pos.y = remainder(this->bg_layer3_pos.y, field_triggers_header_ptr->bg3_height);
	this->bg_layer3_pos.y = this->bg_layer3_pos.y + 232 - ff7_externals.field_bg_offset->y - ff7_externals.modules_global_object->shake_bg_y.shake_curr_value;
	this->bg_layer4_pos.x = (field_triggers_header_ptr->bg4_pos_x / 16.f) + ((field_triggers_header_ptr->bg4_speed_x * delta_position.x) / 256.f);
	this->bg_layer4_pos.x = remainder(this->bg_layer4_pos.x, field_triggers_header_ptr->bg4_width);
	this->bg_layer4_pos.x = this->bg_layer4_pos.x + 320 - ff7_externals.field_bg_offset->x - ff7_externals.modules_global_object->shake_bg_x.shake_curr_value;
	this->bg_layer4_pos.y = (field_triggers_header_ptr->bg4_pos_y / 16.f) + ((field_triggers_header_ptr->bg4_speed_y * delta_position.y) / 256.f);
	this->bg_layer4_pos.y = remainder(this->bg_layer4_pos.y, field_triggers_header_ptr->bg4_height);
	this->bg_layer4_pos.y = this->bg_layer4_pos.y + 232 - ff7_externals.field_bg_offset->y - ff7_externals.modules_global_object->shake_bg_y.shake_curr_value;

	// Round the position to steps of 1/MIN_STEP_INVERSE due to visual glitches between tiles
	this->field_3d_world_pos.x = round(this->field_3d_world_pos.x * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
	this->field_3d_world_pos.y = round(this->field_3d_world_pos.y * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
	this->bg_main_layer_pos.x = round(this->bg_main_layer_pos.x * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
	this->bg_main_layer_pos.y = round(this->bg_main_layer_pos.y * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
	this->bg_layer3_pos.x = round(this->bg_layer3_pos.x * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
	this->bg_layer3_pos.y = round(this->bg_layer3_pos.y * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
	this->bg_layer4_pos.x = round(this->bg_layer4_pos.x * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
	this->bg_layer4_pos.y = round(this->bg_layer4_pos.y * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
}

template<typename T>
void FieldScrollingEngine<T>::field_set_bg_values()
{
	//TODO: implement
}

template<typename T>
void FieldScrollingEngine<T>::field_update_scripted_bg_movement()
{
	vector2<T> world_pos;
	if(ff7_externals.modules_global_object->world_move_status == 1)
	{
		switch(ff7_externals.modules_global_object->world_move_mode)
		{
		case 1:
            this->set_world_pos_based_on_player_pos<T>(&world_pos, ff7_externals.modules_global_object->world_move_follow_model_id);
            this->clip_with_camera_range<T>(&world_pos);
            this->curr_delta_world_pos.x = -world_pos.x;
            this->curr_delta_world_pos.y = -world_pos.y;
			break;
		case 2:
		case 3:
			if(*ff7_externals.scripted_world_move_n_steps)
			{
                this->set_world_pos_based_on_player_pos<T>(&world_pos, ff7_externals.modules_global_object->world_move_follow_model_id);
                this->clip_with_camera_range<T>(&world_pos);
                std::function<T(T, T, int, int)> field_get_interpolated_value = ff7_externals.modules_global_object->world_move_mode == 2 ?
					field_get_linear_interpolated_value<T> : field_get_smooth_interpolated_value<T>;
                this->curr_delta_world_pos.x = field_get_interpolated_value(
                    this->initial_delta_world_pos.x,
                    -world_pos.x,
                    *ff7_externals.scripted_world_move_n_steps,
                    *ff7_externals.scripted_world_move_step_index
                );
                this->curr_delta_world_pos.y = field_get_interpolated_value(
                    this->initial_delta_world_pos.y,
                    -world_pos.y,
                    *ff7_externals.scripted_world_move_n_steps,
                    *ff7_externals.scripted_world_move_step_index
                );

				if(*ff7_externals.scripted_world_move_n_steps == *ff7_externals.scripted_world_move_step_index)
					ff7_externals.modules_global_object->world_move_status = 2;
				else
					(*ff7_externals.scripted_world_move_step_index)++;
			}
			else
			{
				ff7_externals.modules_global_object->world_move_status = 2;
			}
			break;
		case 5:
		case 6:
			if(*ff7_externals.scripted_world_move_n_steps)
			{
                std::function<T(T, T, int, int)> field_get_interpolated_value = ff7_externals.modules_global_object->world_move_mode == 5 ?
                    field_get_linear_interpolated_value<T> : field_get_smooth_interpolated_value<T>;
                this->curr_delta_world_pos.x = field_get_interpolated_value(
                    this->initial_delta_world_pos.x,
                    this->final_delta_world_pos.x,
                    *ff7_externals.scripted_world_move_n_steps,
                    *ff7_externals.scripted_world_move_step_index
                );
                this->curr_delta_world_pos.y = field_get_interpolated_value(
                    this->initial_delta_world_pos.y,
                    this->final_delta_world_pos.y,
                    *ff7_externals.scripted_world_move_n_steps,
                    *ff7_externals.scripted_world_move_step_index
                );

				if(*ff7_externals.scripted_world_move_n_steps == *ff7_externals.scripted_world_move_step_index)
					ff7_externals.modules_global_object->world_move_status = 2;
				else
					(*ff7_externals.scripted_world_move_step_index)++;
			}
			else
			{
				ff7_externals.modules_global_object->world_move_status = 2;
			}
			break;
		default:
			break;
		}
	}
}

template<typename T>
void FieldScrollingEngine<T>::ff7_field_update_background()
{
	this->field_3d_world_pos = {INVALID_VALUE, INVALID_VALUE};
	this->bg_main_layer_pos = {INVALID_VALUE, INVALID_VALUE};
	this->bg_layer3_pos = {INVALID_VALUE, INVALID_VALUE};
	this->bg_layer4_pos = {INVALID_VALUE, INVALID_VALUE};

	field_trigger_header* field_triggers_header_ptr = *ff7_externals.field_triggers_header;
	field_triggers_header_ptr->bg3_pos_x += ff7_externals.modules_global_object->bg3_scroll_speed_x;
	field_triggers_header_ptr->bg3_pos_y += ff7_externals.modules_global_object->bg3_scroll_speed_y;
	field_triggers_header_ptr->bg3_pos_x %= 16 * field_triggers_header_ptr->bg3_width;
	field_triggers_header_ptr->bg3_pos_y %= 16 * field_triggers_header_ptr->bg3_height;
	field_triggers_header_ptr->bg4_pos_x += ff7_externals.modules_global_object->bg4_scroll_speed_x;
	field_triggers_header_ptr->bg4_pos_y += ff7_externals.modules_global_object->bg4_scroll_speed_y;
	field_triggers_header_ptr->bg4_pos_x %= 16 * field_triggers_header_ptr->bg4_width;
	field_triggers_header_ptr->bg4_pos_y %= 16 * field_triggers_header_ptr->bg4_height;
	ff7_externals.engine_sub_661A1C(ff7_externals.field_camera_data->zoom); // TODO: get 661A1AC
	if ( *ff7_externals.word_CC1638 && !ff7_externals.modules_global_object->BGMOVIE_flag)
	{
		*ff7_externals.field_flag_CFF28C = 1; // TODO get this
		int field_bg_multiplier = *ff7_externals.field_bg_multiplier;
		if(ff7_externals.modules_global_object->MVCAM_flag == 1)
		{
			this->field_3d_world_pos.x = (this->curr_delta_world_pos.x + ff7_externals.field_bg_offset->x - 160) * field_bg_multiplier;
			this->field_3d_world_pos.y = (this->curr_delta_world_pos.y + ff7_externals.field_bg_offset->y - 120) * field_bg_multiplier;
			this->field_3d_world_pos.x = round(this->field_3d_world_pos.x * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
			this->field_3d_world_pos.y = round(this->field_3d_world_pos.y * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
		}
		else
		{
			this->field_3d_world_pos.x = (ff7_externals.field_bg_offset->x - ff7_externals.field_camera_data->pan_x - 160) * field_bg_multiplier;
			this->field_3d_world_pos.y = (ff7_externals.field_bg_offset->y + ff7_externals.field_camera_data->pan_y - 120) * field_bg_multiplier;
			this->field_3d_world_pos.x = round(this->field_3d_world_pos.x * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
			this->field_3d_world_pos.y = round(this->field_3d_world_pos.y * MIN_STEP_INVERSE) / MIN_STEP_INVERSE;
		}
	}
	else if(*ff7_externals.field_bg_flag_CC15E4)
	{
		vector2<T> delta_pos;
		delta_pos.x = -this->curr_delta_world_pos.x;
		delta_pos.y = -this->curr_delta_world_pos.y;
		this->set_world_and_background_positions<T>(delta_pos, true);
		if((*ff7_externals.field_event_data_ptr)[*ff7_externals.field_player_model_id].field_62)
			this->set_special_position_with_player_pos();
	}
	else
	{

		if(ff7_externals.modules_global_object->camera_phase) // TODO: change the values for modules_global_object
		{
			if(ff7_externals.modules_global_object->camera_phase == 1)
			{
				ff7_externals.modules_global_object->curr_value = field_get_linear_interpolated_value<int>(
					ff7_externals.modules_global_object->initial_value,
					ff7_externals.modules_global_object->final_value,
					ff7_externals.modules_global_object->n_steps,
					ff7_externals.modules_global_object->step_index
				);
				if(ff7_externals.modules_global_object->step_index == ff7_externals.modules_global_object->n_steps)
					ff7_externals.modules_global_object->camera_phase = 3;
				else
					++ff7_externals.modules_global_object->step_index;
			}
			else if(ff7_externals.modules_global_object->camera_phase == 2)
			{
				ff7_externals.modules_global_object->curr_value = field_get_smooth_interpolated_value<int>(
					ff7_externals.modules_global_object->initial_value,
					ff7_externals.modules_global_object->final_value,
					ff7_externals.modules_global_object->n_steps,
					ff7_externals.modules_global_object->step_index
				);
				if(ff7_externals.modules_global_object->step_index == ff7_externals.modules_global_object->n_steps)
					ff7_externals.modules_global_object->camera_phase = 3;
				else
					++ff7_externals.modules_global_object->step_index;
			}
		}
		vector2<T> bg_delta_position;
		this->set_world_pos_based_on_player_pos<T>(&bg_delta_position, *ff7_externals.field_player_model_id);
		this->clip_with_camera_range(&bg_delta_position);
		this->clip_layer3_camera_range(*ff7_externals.field_triggers_header, &bg_delta_position);
		this->curr_delta_world_pos.x = -bg_delta_position.x;
		this->curr_delta_world_pos.y = -bg_delta_position.y;
		this->set_world_and_background_positions<T>(bg_delta_position, false);
		this->set_special_position_with_player_pos(); // TODO: implement this
	}
}