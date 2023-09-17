#include <safetyhook.hpp>
#include <spdlog/spdlog.h>
#include "offsets.h"

auto active = false;
auto retrying = false;

// Calculate the best possible score we can get given our current progress in the chart.
inline auto get_max_score(std::uint32_t note_current, std::uint32_t note_total, std::uint32_t score) -> std::uint32_t
    { return (score + ((note_total - note_current) * 2)); }

// Handle the core functionality, as well as displaying text in the pacemaker frame.
auto pacemaker_text_hook_fn(SafetyHookContext& ctx) -> void
{
    // Toggle auto-retry when the 5 key is pressed by an active player.
    auto static was_down = false;
    auto const is_down =
        (!dead_state->p1 && test_10key(0, 5)) ||
        (!dead_state->p2 && test_10key(1, 5));

    if (is_down && !was_down)
        active = !active;

    was_down = is_down;

    // Nothing to do.
    if (!active)
        return;

    // Do nothing if there are no active players.
    if (dead_state->p1 && dead_state->p2)
        return;

    // Similarly, do nothing if both players are active.
    // In this situation, the pacemaker can not be used.
    if (!dead_state->p1 && !dead_state->p2)
        return;

    // Nothing to do if there's no target score.
    if (*pacemaker_score_target == 0)
        return;

    // Various score related variables for the active player.
    auto const score = (!dead_state->p1 ? *p1_score_current: *p2_score_current);
    auto const note_current = (!dead_state->p1 ? *p1_note_current: *p2_note_current);
    auto const note_total = (!dead_state->p1 ? *p1_note_total: *p2_note_total);

    // Given our current progress in the chart, calculate the best possible score we can get.
    auto const best_score = get_max_score(note_current, note_total, score);

    // If the target is above this, we can no longer clear.
    // Set the auto-retry flag and fail the stage.
    if (!retrying && best_score < *pacemaker_score_target)
    {
        retrying = true;
        dead_state->p1 = true;
        dead_state->p2 = true;
    }

    // Don't render text anymore if we've already failed.
    if (retrying)
        return;

    // Calculate how much EX score we can still lose before it becomes impossible to clear.
    auto const delta = -(*pacemaker_score_target - best_score);

    // Display it in the corner of the score graph.
    auto const x = static_cast<int>(ctx.rdx);
    auto const y = static_cast<int>(ctx.rbp);
    draw_text(2, x, y, 88, pacemaker_text_obj, " DELTA:+%04d", delta);

    // If the stage has ended, the "LV. X:+0000" text will be rendered after this hook.
    // Adjust the text vertical offset so that it doesn't render on top of our text.
    ctx.rbp -= 20;
}

// First hook where the game checks if EFFECT is held.
// If we're auto-retrying, simulate the button being held.
auto retry_btn_check_a_hook_fn(SafetyHookContext& ctx) -> void
{
    if (retrying)
        ctx.rflags &= ~0x40;
}

// Second hook where the game checks if VEFX is held.
// After this is called, an attempt will be made to quick retry.
auto retry_btn_check_b_hook_fn(SafetyHookContext& ctx) -> void
{
    if (retrying)
        ctx.rflags &= ~0x40;
    retrying = false;
}

// Prevent the 'stage failed' animation from being played when auto-retrying.
auto play_failed_animation_hook_fn(SafetyHookContext& ctx) -> void
    { ctx.rdx = !retrying; }

// Prevent the 'stage failed' system sound from being played when auto-retrying.
auto mute_failed_sound_hook_fn(SafetyHookContext& ctx) -> void
    { ctx.rcx = (retrying ? 0: ctx.rcx); }

// Skip the 'stage failed' animation and fade out immediately when auto-retrying.
auto failed_duration_hook_fn(SafetyHookContext& ctx) -> void
{
    if (retrying)
        ctx.rflags = (ctx.rflags | (1ULL << 11)) & ~(1ULL << 6);
}

auto DllMain(HMODULE module, unsigned long reason, void*) -> int try
{
    if (reason != DLL_PROCESS_ATTACH)
        return TRUE;

    // Scan for all the game addresses we need.
    // If anything can't be found, give up and detach.
    resolve_offsets();

    // Install a bunch of hooks.
    static auto pacemaker_text_hook = safetyhook::create_mid(pacemaker_text_hook_addr, pacemaker_text_hook_fn);
    static auto retry_btn_check_a_hook = safetyhook::create_mid(retry_btn_check_a_hook_addr, retry_btn_check_a_hook_fn);
    static auto retry_btn_check_b_hook = safetyhook::create_mid(retry_btn_check_b_hook_addr, retry_btn_check_b_hook_fn);
    static auto play_failed_animation_hook = safetyhook::create_mid(play_failed_animation_hook_addr, play_failed_animation_hook_fn);
    static auto mute_failed_sound_hook = safetyhook::create_mid(mute_failed_sound_hook_addr, mute_failed_sound_hook_fn);

    // This hook simulates the 'Quicker Quick Retry' hex edit when auto-retrying.
    // Don't bother hooking it if the code has already been patched to always skip the animation.
    if (*reinterpret_cast<std::uint16_t*>(failed_duration_hook_addr) != 0x9090)
        static auto failed_transition_hook = safetyhook::create_mid(failed_duration_hook_addr, failed_duration_hook_fn);

    spdlog::info("2dxAutoRetry initialized successfully.");
    return TRUE;
}
catch (std::exception const& e)
{
    spdlog::error("{}", e.what());
    return FALSE;
}