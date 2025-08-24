#include "graphics.hpp"

#include <string>
#include <unordered_map>

#include "raylib.h"
#include "runtime/value.hpp"

LiteralValue *init_window(Memory &memory,
                          const std::vector<LiteralValue *> &args) {
    expect_args(args, 3);

    InitWindow(
        as_double(args[0]), as_double(args[1]), as_string(args[2]).c_str());
    SetTargetFPS(60);
    return nullptr;
}

LiteralValue *close_window(Memory &memory,
                           const std::vector<LiteralValue *> &args) {
    expect_args(args, 0);
    CloseWindow();
    return nullptr;
}

LiteralValue *window_should_close(Memory &memory,
                                  const std::vector<LiteralValue *> &args) {
    expect_args(args, 0);
    return memory.get<BoolValue>(WindowShouldClose());
}

LiteralValue *begin_drawing(Memory &memory,
                            const std::vector<LiteralValue *> &args) {
    expect_args(args, 0);
    BeginDrawing();
    return nullptr;
}

LiteralValue *end_drawing(Memory &memory,
                          const std::vector<LiteralValue *> &args) {
    expect_args(args, 0);
    EndDrawing();
    return nullptr;
}

LiteralValue *clear_background(Memory &memory,
                               const std::vector<LiteralValue *> &args) {
    expect_args(args, 1);
    const std::string &color = static_cast<StringValue *>(args[0])->value;
    ClearBackground(str_to_color(color));
    return nullptr;
}

LiteralValue *draw_rectangle(Memory &memory,
                             const std::vector<LiteralValue *> &args) {
    expect_args(args, 5);

    DrawRectangle(as_double(args[0]),
                  as_double(args[1]),
                  as_double(args[2]),
                  as_double(args[3]),
                  str_to_color(as_string(args[4])));
    return nullptr;
}

LiteralValue *draw_circle(Memory &memory,
                          const std::vector<LiteralValue *> &args) {
    expect_args(args, 4);
    DrawCircleV({(float)as_double(args[0]), (float)as_double(args[1])},
                as_double(args[2]),
                str_to_color(as_string(args[3])));
    return nullptr;
}

LiteralValue *draw_text(Memory &memory,
                        const std::vector<LiteralValue *> &args) {
    expect_args(args, 5);
    const std::string &text = as_string(args[0]);
    DrawText(text.c_str(),
             as_double(args[1]),
             as_double(args[2]),
             as_double(args[3]),
             str_to_color(as_string(args[4])));
    return nullptr;
}

std::unordered_map<std::string, int> key_codes = {
    {"down", KEY_DOWN},
    {"up", KEY_UP},
    {"left", KEY_LEFT},
    {"right", KEY_RIGHT},
};

LiteralValue *is_key_down(Memory &memory,
                          const std::vector<LiteralValue *> &args) {
    expect_args(args, 1);
    const std::string &key = as_string(args[0]);
    return memory.get<BoolValue>(IsKeyDown(key_codes[key]));
}

void expect_args(const std::vector<LiteralValue *> &args, size_t count) {
    if (args.size() != count) {
        fmt::println("UH OH");
        throw Error(
            Error::ARGUMENT_ERROR,
            fmt::format(
                "Expected {} arguments but got {}.", count, args.size()));
    }
}

std::unordered_map<std::string, Color> color_map = {
    {"white", WHITE},
    {"black", BLACK},
    {"red", RED},
    {"orange", ORANGE},
    {"yellow", YELLOW},
    {"blue", BLUE},
    {"brown", BROWN},
    {"maroon", MAROON},
    {"green", GREEN},
};

Color str_to_color(const std::string &color) {
    if (color_map.contains(color)) {
        return color_map[color];
    }
    return BLACK;
}
