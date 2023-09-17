#pragma once

#include <cstdint>

struct dead_state_t
{
    bool p1;
    bool p2;
};

extern std::uint8_t* pacemaker_text_hook_addr;
extern std::uint8_t* retry_btn_check_a_hook_addr;
extern std::uint8_t* retry_btn_check_b_hook_addr;
extern std::uint8_t* play_failed_animation_hook_addr;
extern std::uint8_t* mute_failed_sound_hook_addr;
extern std::uint8_t* failed_duration_hook_addr;

extern bool (*test_10key) (std::uint32_t player, std::uint32_t key);
extern void* (*draw_text) (int font, int x, int y, int flags, void* obj, const char* fmt, ...);

extern void* pacemaker_text_obj;
extern dead_state_t* dead_state;
extern std::uint32_t* pacemaker_score_target;
extern std::uint32_t* p1_score_current;
extern std::uint32_t* p2_score_current;
extern std::uint32_t* p1_note_current;
extern std::uint32_t* p2_note_current;
extern std::uint32_t* p1_note_total;
extern std::uint32_t* p2_note_total;

void resolve_offsets();