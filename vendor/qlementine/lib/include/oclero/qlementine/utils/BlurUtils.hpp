// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

// Source: https://github.com/bfraboni/FastGaussianBlur/blob/main/fast_gaussian_blur_template.h

#pragma once

#include <cmath>
#include <cassert>
#include <algorithm>
#include <array>

namespace oclero::qlementine {
enum class EdgePolicy {
  Extend,
  Crop,
};

template<typename T, int C, EdgePolicy P = EdgePolicy::Extend>
void horizontal_blur(const T* in, T* out, const int w, const int h, const int r) {
  double iarr = 1. / (r + r + 1);
  for (int i = 0; i < h; i++) {
    int ti = i * w;
    int li = ti;
    int ri = ti + r;
    std::array<double, C> fv, lv, val;

    for (int ch = 0; ch < C; ++ch) {
      fv[ch] = P == EdgePolicy::Extend ? in[ti * C + ch] : 0; // unused with Crop policy
      lv[ch] = P == EdgePolicy::Extend ? in[(ti + w - 1) * C + ch] : 0; // unused with kcrop policy
      val[ch] = P == EdgePolicy::Extend ? (r + 1) * fv[ch] : 0;
    }

    // initial acucmulation
    for (int j = 0; j < r; j++) {
      for (int ch = 0; ch < C; ++ch) {
        val[ch] += in[(ti + j) * C + ch];
      }
    }

    // left border - filter kernel is incomplete
    for (int j = 0; j <= r; j++, ri++, ti++) {
      for (int ch = 0; ch < C; ++ch) {
        val[ch] += P == EdgePolicy::Extend ? in[ri * C + ch] - fv[ch] : in[ri * C + ch];
        out[ti * C + ch] = P == EdgePolicy::Extend ? val[ch] * iarr : val[ch] / (r + j + 1);
      }
    }

    // center of the image - filter kernel is complete
    for (int j = r + 1; j < w - r; j++, ri++, ti++, li++) {
      for (int ch = 0; ch < C; ++ch) {
        val[ch] += in[ri * C + ch] - in[li * C + ch];
        out[ti * C + ch] = val[ch] * iarr;
      }
    }

    // right border - filter kernel is incomplete
    for (int j = w - r; j < w; j++, ti++, li++) {
      for (int ch = 0; ch < C; ++ch) {
        val[ch] += P == EdgePolicy::Extend ? lv[ch] - in[li * C + ch] : -in[li * C + ch];
        out[ti * C + ch] = P == EdgePolicy::Extend ? val[ch] * iarr : val[ch] / (r + w - j);
      }
    }
  }
}

template<typename T>
void horizontal_blur(const T* in, T* out, const int w, const int h, const int channelCount, const int r) {
  switch (channelCount) {
    case 1:
      horizontal_blur<T, 1>(in, out, w, h, r);
      break;
    case 2:
      horizontal_blur<T, 2>(in, out, w, h, r);
      break;
    case 3:
      horizontal_blur<T, 3>(in, out, w, h, r);
      break;
    case 4:
      horizontal_blur<T, 4>(in, out, w, h, r);
      break;
    default:
      assert(((void) "Number of channels not supported", false)); // NOLINT
      break;
  }
}

template<typename T, int C>
void flip_block(const T* in, T* out, const int w, const int h) {
  constexpr int block = 256 / C;
  for (int x = 0; x < w; x += block) {
    for (int y = 0; y < h; y += block) {
      const T* p = in + y * w * C + x * C;
      T* q = out + y * C + x * h * C;

      const int blockx = std::min(w, x + block) - x;
      const int blocky = std::min(h, y + block) - y;
      for (int xx = 0; xx < blockx; xx++) {
        for (int yy = 0; yy < blocky; yy++) {
          for (int k = 0; k < C; k++) {
            q[k] = p[k];
          }
          p += w * C;
          q += C;
        }
        p += -blocky * w * C + C;
        q += -blocky * C + h * C;
      }
    }
  }
}

template<typename T>
void flip_block(const T* in, T* out, const int w, const int h, const int channelCount) {
  switch (channelCount) {
    case 1:
      flip_block<T, 1>(in, out, w, h);
      break;
    case 2:
      flip_block<T, 2>(in, out, w, h);
      break;
    case 3:
      flip_block<T, 3>(in, out, w, h);
      break;
    case 4:
      flip_block<T, 4>(in, out, w, h);
      break;
    default:
      assert(((void) "Number of channels not supported", false)); // NOLINT
      break;
  }
}

inline void sigma_to_box_radius(int* boxes, const double sigma, const int passCount) {
  // ideal filter width
  double wi = std::sqrt((12 * sigma * sigma / passCount) + 1);
  int wl = int(wi); // no need std::floor
  if (wl % 2 == 0)
    wl--;
  int wu = wl + 2;

  double mi = (12 * sigma * sigma - passCount * wl * wl - 4 * passCount * wl - 3 * passCount) / (-4 * wl - 4);
  int m = mi + .5; // NOLINT (avoid std::round by adding 0.5f and cast to integer type)

  for (int i = 0; i < passCount; i++) {
    boxes[i] = ((i < m ? wl : wu) - 1) / 2;
  }
}

template<typename T, unsigned int N>
void fast_gaussian_blur(T*& in, T*& out, const int w, const int h, const int channelCount, const double sigma) {
  // compute box kernel sizes
  std::array<int, N> boxes;
  sigma_to_box_radius(boxes.data(), sigma, N);

  // perform N horizontal blur passes
  for (int i = 0; i < N; ++i) {
    horizontal_blur(in, out, w, h, channelCount, boxes[i]);
    std::swap(in, out);
  }

  // flip buffer
  flip_block(in, out, w, h, channelCount);
  std::swap(in, out);

  // perform N horizontal blur passes on flipped image
  for (int i = 0; i < N; ++i) {
    horizontal_blur(in, out, h, w, channelCount, boxes[i]);
    std::swap(in, out);
  }

  // flip buffer
  flip_block(in, out, h, w, channelCount);
}

template<typename T>
void fast_gaussian_blur(T*& in, T*& out, const int w, const int h, const int channelCount, const double sigma) {
  // compute box kernel sizes
  std::array<int, 3> boxes{};
  sigma_to_box_radius(boxes.data(), sigma, 3);

  // perform 3 horizontal blur passes
  horizontal_blur(in, out, w, h, channelCount, boxes[0]);
  horizontal_blur(out, in, w, h, channelCount, boxes[1]);
  horizontal_blur(in, out, w, h, channelCount, boxes[2]);

  // flip buffer
  flip_block(out, in, w, h, channelCount);

  // perform 3 horizontal blur passes on flipped image
  horizontal_blur(in, out, h, w, channelCount, boxes[0]);
  horizontal_blur(out, in, h, w, channelCount, boxes[1]);
  horizontal_blur(in, out, h, w, channelCount, boxes[2]);

  // flip buffer
  flip_block(out, in, h, w, channelCount);

  // swap pointers to get result in the output buffer
  std::swap(in, out);
}

template<typename T>
void fast_gaussian_blur(
  T*& in, T*& out, const int w, const int h, const int channelCount, const double sigma, const unsigned int passCount) {
  switch (passCount) {
    case 1:
      fast_gaussian_blur<T, 1>(in, out, w, h, channelCount, sigma);
      break;
    case 2:
      fast_gaussian_blur<T, 2>(in, out, w, h, channelCount, sigma);
      break;
    case 3:
      // specialized 3 passes version
      fast_gaussian_blur<T>(in, out, w, h, channelCount, sigma);
      break;
    case 4:
      fast_gaussian_blur<T, 4>(in, out, w, h, channelCount, sigma);
      break;
    case 5:
      fast_gaussian_blur<T, 5>(in, out, w, h, channelCount, sigma);
      break;
    case 6:
      fast_gaussian_blur<T, 6>(in, out, w, h, channelCount, sigma);
      break;
    case 7:
      fast_gaussian_blur<T, 7>(in, out, w, h, channelCount, sigma);
      break;
    case 8:
      fast_gaussian_blur<T, 8>(in, out, w, h, channelCount, sigma);
      break;
    case 9:
      fast_gaussian_blur<T, 9>(in, out, w, h, channelCount, sigma);
      break;
    case 10:
      fast_gaussian_blur<T, 10>(in, out, w, h, channelCount, sigma);
      break;
    default:
      assert(((void) "Number of passes not supported", false)); // NOLINT
      break;
  }
}

// Source: https://stackoverflow.com/questions/21984405/relation-between-sigma-and-radius-on-the-gaussian-blur
constexpr auto pixelToSigma = 2.329; // std::sqrt(2 * std::log(255)) - 1;
} // namespace oclero::qlementine
