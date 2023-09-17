#include "offsets.h"
#include "util/module.h"
#include "spdlog/spdlog.h"

decltype(pacemaker_text_hook_addr) pacemaker_text_hook_addr = nullptr;
decltype(retry_btn_check_a_hook_addr) retry_btn_check_a_hook_addr = nullptr;
decltype(retry_btn_check_b_hook_addr) retry_btn_check_b_hook_addr = nullptr;
decltype(play_failed_animation_hook_addr) play_failed_animation_hook_addr = nullptr;
decltype(mute_failed_sound_hook_addr) mute_failed_sound_hook_addr = nullptr;
decltype(failed_duration_hook_addr) failed_duration_hook_addr = nullptr;

decltype(test_10key) test_10key = nullptr;
decltype(draw_text) draw_text = nullptr;

decltype(pacemaker_text_obj) pacemaker_text_obj = nullptr;
decltype(dead_state) dead_state = nullptr;
decltype(pacemaker_score_target) pacemaker_score_target = nullptr;
decltype(p1_score_current) p1_score_current = nullptr;
decltype(p2_score_current) p2_score_current = nullptr;
decltype(p1_note_current) p1_note_current = nullptr;
decltype(p2_note_current) p2_note_current = nullptr;
decltype(p1_note_total) p1_note_total = nullptr;
decltype(p2_note_total) p2_note_total = nullptr;

auto resolve_offsets() -> void
{
    auto bm2dx = util::module {};

    // Put any possible game module filenames here.
    // This will probably be configurable in the future.
    for (auto const& name: {"bm2dx.dll", "bm2dx_omni.dll"})
        if ((bm2dx = util::module{name}))
            break;

    if (!bm2dx)
        throw std::runtime_error("Failed to find game module.");

    // These addresses are specifically for the final '2022082400' version.
    pacemaker_text_hook_addr = reinterpret_cast<decltype(pacemaker_text_hook_addr)>(*bm2dx + 0x428C79);
    retry_btn_check_a_hook_addr = reinterpret_cast<decltype(retry_btn_check_a_hook_addr)>(*bm2dx + 0x4347FD);
    retry_btn_check_b_hook_addr = reinterpret_cast<decltype(retry_btn_check_b_hook_addr)>(*bm2dx + 0x43480F);
    play_failed_animation_hook_addr = reinterpret_cast<decltype(play_failed_animation_hook_addr)>(*bm2dx + 0x434DBE);
    mute_failed_sound_hook_addr = reinterpret_cast<decltype(mute_failed_sound_hook_addr)>(*bm2dx + 0x434DE8);
    failed_duration_hook_addr = reinterpret_cast<decltype(failed_duration_hook_addr)>(*bm2dx + 0x4349C2);

    test_10key = reinterpret_cast<decltype(test_10key)>(*bm2dx + 0x336A10);
    draw_text = reinterpret_cast<decltype(draw_text)>(*bm2dx + 0x2AC530);

    pacemaker_text_obj = reinterpret_cast<decltype(pacemaker_text_obj)>(*bm2dx + 0x6134EF0);
    dead_state = reinterpret_cast<decltype(dead_state)>(*bm2dx + 0x6132F50);
    pacemaker_score_target = reinterpret_cast<decltype(pacemaker_score_target)>(*bm2dx + 0x5DA13E0);
    p1_score_current = reinterpret_cast<decltype(p1_score_current)>(*bm2dx + 0x5DA09C8);
    p2_score_current = reinterpret_cast<decltype(p2_score_current)>(*bm2dx + 0x5DA0D24);
    p1_note_current = reinterpret_cast<decltype(p1_note_current)>(*bm2dx + 0x5DA09D0);
    p2_note_current = reinterpret_cast<decltype(p2_note_current)>(*bm2dx + 0x5DA0D2C);
    p1_note_total = reinterpret_cast<decltype(p1_note_total)>(*bm2dx + 0x5DA09D4);
    p2_note_total = reinterpret_cast<decltype(p2_note_total)>(*bm2dx + 0x5DA0D30);
}