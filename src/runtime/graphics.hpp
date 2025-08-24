#ifndef RUNTIME_GRAPHICS_HPP
#define RUNTIME_GRAPHICS_HPP

#include <raylib.h>

#include <string>
#include <vector>

#include "runtime/memory.hpp"
#include "runtime/value.hpp"
#include "util/error.hpp"

LiteralValue *init_window(Memory &memory,
                          const std::vector<LiteralValue *> &args);
LiteralValue *close_window(Memory &memory,
                           const std::vector<LiteralValue *> &args);
LiteralValue *window_should_close(Memory &memory,
                                  const std::vector<LiteralValue *> &args);
LiteralValue *begin_drawing(Memory &memory,
                            const std::vector<LiteralValue *> &args);
LiteralValue *end_drawing(Memory &memory,
                          const std::vector<LiteralValue *> &args);
LiteralValue *clear_background(Memory &memory,
                               const std::vector<LiteralValue *> &args);
// drawing funcs
LiteralValue *draw_rectangle(Memory &memory,
                             const std::vector<LiteralValue *> &args);
LiteralValue *draw_circle(Memory &memory,
                          const std::vector<LiteralValue *> &args);
LiteralValue *draw_text(Memory &memory,
                        const std::vector<LiteralValue *> &args);

// input funcs
LiteralValue *is_key_down(Memory &memory,
                          const std::vector<LiteralValue *> &args);

void expect_args(const std::vector<LiteralValue *> &args, size_t count);

Color str_to_color(const std::string &color);

#endif // RUNTIME_GRAPHICS_HPP
