#include "Noise.h"

#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>

double PerlinNoise::fade(double t) // quintic curve
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}
double PerlinNoise::lerp(double a, double b, double t)
{
	return a + t * (b - a);
}
std::pair<double, double> PerlinNoise::randCorner(int x, int y)
{
	int v = static_cast<int>(((x * 1836311903) ^ (y * 2971215073) + 4807526976) & 255);
	v = this->p[v] & 3;
	switch (v)
	{
	case 0:
		return {1, 0};
	case 1:
		return {-1, 0};
	case 2:
		return {0, 1};
	default:
		return {0, -1};
	}
}

double PerlinNoise::Dot(std::pair<double, double> a, std::pair<double, double> b) // ��������� ������������
{
	return a.first * b.first + a.second * b.second;
}
double PerlinNoise::grad(int hash, double x, double y, double z)
{
	int h = hash & 15;
	double u = h < 8 ? x : y,
		   v = h < 4 ? y : h == 12 || h == 14 ? x
											  : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
PerlinNoise::PerlinNoise(unsigned int seed)
{
	this->p.resize(256);
	std::iota(p.begin(), p.end(), 0);
	std::default_random_engine engine(seed);
	std::shuffle(p.begin(), p.end(), engine);
	this->p.insert(p.end(), p.begin(), p.end());
}
double PerlinNoise::Noise(float fx, float fy)
{
	int left = (int)floor(fx);
	int top = (int)floor(fy);

	double pointInQuadX = fx - left;
	double pointInQuadY = fy - top;
	std::pair<double, double> tLG = this->randCorner(left, top);
	std::pair<double, double> tRG = this->randCorner(left + 1, top);
	std::pair<double, double> bLG = this->randCorner(left, top + 1);
	std::pair<double, double> bRG = this->randCorner(left + 1, top + 1);
	std::pair<double, double> distanceToTopLeft = {pointInQuadX, pointInQuadY};
	std::pair<double, double> distanceToTopRight = {pointInQuadX - 1, pointInQuadY};
	std::pair<double, double> distanceToBottomLeft = {pointInQuadX, pointInQuadY - 1};
	std::pair<double, double> distanceToBottomRight = {pointInQuadX - 1, pointInQuadY - 1};

	double tx1 = this->Dot(distanceToTopLeft, tLG);
	double tx2 = this->Dot(distanceToTopRight, tRG);
	double bx1 = this->Dot(distanceToBottomLeft, bLG);
	double bx2 = this->Dot(distanceToBottomRight, bRG);

	pointInQuadX = this->fade(pointInQuadX);
	pointInQuadY = this->fade(pointInQuadY);

	double tx = this->lerp(tx1, tx2, pointInQuadX);
	double bx = this->lerp(bx1, bx2, pointInQuadX);
	double tb = this->lerp(tx, bx, pointInQuadY);

	return tb;
}

double PerlinNoise::Noise(float fx, float fy, int octaves, float persistence = 0.5f)
{
	float amplitude = 1;
	float max = 0;
	float result = 0;

	while (octaves-- > 0)
	{
		max += amplitude;
		result += this->Noise(fx, fy) * amplitude;
		amplitude *= persistence;
		fx *= 2;
		fy *= 2;
	}
	return result / max;
}

double PerlinNoise::Cloud(float x, float y, int octaves, float persistence)
{
	return this->Noise(x, y, octaves, persistence);
}

double PerlinNoise::Wood(float x, float y, int octaves, float persistence)
{
	double n = 20 * this->Noise(x, y, octaves, persistence);
	n -= floor(n);
	return n;
}

double PerlinNoise::Khaki(float x, float y, int octaves, float persistence)
{
	double n = 50 * this->Noise(x, y, octaves, persistence);
	n += floor(n);
	return n;
}

double PerlinNoise::Malachite(float x, float y, int octaves, float persistence)
{
	double n = 20 * this->Noise(x, y, octaves, persistence);
	n -= floor(n);
	return sin(n) * sin(n) * cos(n) * cos(n);
}

double PerlinNoise::idk(float x, float y, int octaves, float persistence)
{
	double n = 5 * this->Noise(x, y, octaves, persistence);
	n += n;

	return cos(n) * sin(n) - sin(n);

	// double n = 5 * this->Noise(x, y, octaves, persistence);
	// n += n;

	// return sin(n);
}

double min(double l, double r)
{
	return l <= r ? l : r;
}

double max(double l, double r)
{
	return l >= r ? l : r;
}

std::vector<int> PerlinNoise::getColor(int id, int initCol)
{
	std::vector<int> col{0x3a1b11, 0x4a271a, 0x5a3323, 0x6a3f2c, 0x774934, 0x84533c, 0x8f5b42, 0x9a6348, 0xa86d4f, 0xb67756, 0xc4805d, 0xd28964, 0xeb9b70, 0xeca47b, 0xeeb691, 0xf0bf9c, 0xf3cfaf, 0xf4dabc};

	if (id == 1)
	{
		for (int i = 15; i < 256; i += 15)
			if (initCol <= i)
				return {(col[i / 15 - 1] >> 16) & 0xFF, (col[i / 15 - 1] >> 8) & 0xFF, col[i / 15 - 1] & 0xFF};
	}

	switch (id)
	{
	case 0:
		return {initCol % 218, initCol, 232}; // Cloud
	case 2:
		return {7, initCol >= 120 ? 120 : initCol < 40 ? 40
													   : initCol,
				0}; // Khaki
	case 3:
		return {initCol - 40, initCol + 40, initCol}; // Malachite
	default:
		return {initCol, initCol, initCol}; // idk
	}
}
