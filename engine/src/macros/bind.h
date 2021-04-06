#pragma once

/*
 * Created by Kami-Kaze on 3/27/2021.
 */

#define BIND_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }