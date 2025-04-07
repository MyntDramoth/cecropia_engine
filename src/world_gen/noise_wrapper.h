#pragma once

#include <FastNoise/FastNoise.h>

auto noise_simplex = FastNoise::New<FastNoise::Simplex>();
auto noise_cellular = FastNoise::New<FastNoise::Cellular>();
auto noise_fractal = FastNoise::New<FastNoise::FractalFBm>();
auto noise_perlin = FastNoise::New<FastNoise::Perlin>();
