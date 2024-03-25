#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include <emmintrin.h>
#include <string>
#include <immintrin.h>
#include <xmmintrin.h>

const int MAX_ITERATIONS = 256;
const __m128 RADIUS = _mm_set_ps1(100.f);

const int WIDTH = 800;
const int HEIGHT = 600;

inline void get_colour(__m128 X0, __m128 Y0, __m128i color) {
    __m128 X =  _mm_set_ps1(0);
    __m128 Y =  _mm_set_ps1(0);


    _mm_storeu_si128((__m128i*)&X, (__m128i)X0);
    _mm_storeu_si128((__m128i*)&Y, (__m128i)Y0);




    for (int n = 0; n < MAX_ITERATIONS; n++) {

        __m128 XX = _mm_mul_ps(X,X);
        __m128 YY = _mm_mul_ps(Y,Y);
        __m128 XY = _mm_mul_ps(X,Y);
        __m128 R2 = _mm_add_ps(YY,XX);


        __m128 cmp = _mm_cmple_ps(R2, RADIUS);
 
        int mask = _mm_movemask_ps((__m128)cmp);

        if (!mask) break;

        color =  _mm_sub_epi32(color, (__m128i)cmp);

        _mm_storeu_si128((__m128i*)&X, (__m128i)_mm_sub_ps(_mm_add_ps(XX,YY), X));
        _mm_storeu_si128((__m128i*)&Y, (__m128i)_mm_add_ps(_mm_add_ps(XY,YY), Y));
    }
}

int main() {

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot Set");

    sf::Image image;
    image.create(WIDTH, HEIGHT, sf::Color::Black);

    sf::Texture texture;
    texture.create(WIDTH, HEIGHT);
    sf::Sprite sprite(texture);

    sf::Font font;
    sf::Text fpsText("", font, 20);
    fpsText.setColor(sf::Color::White);
    fpsText.setPosition(10, 10);

    sf::Clock gameClock; // Переименовано из clock в gameClock
    int frames = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

// ---------------------------------------------------------------------

        // Отрисовка Mandelbrot Set
        for (int y0 = 0; y0 < HEIGHT; y0++) {
            for (int x0 = 0; x0 < WIDTH; x0+=4) {

                __m128 X = _mm_set_ps((float)(x0 + 0) / WIDTH * 3.5f - 2.5f,
                                      (float)(x0 + 1) / WIDTH * 3.5f - 2.5f,
                                      (float)(x0 + 2) / WIDTH * 3.5f - 2.5f,
                                      (float)(x0 + 3) / WIDTH * 3.5f - 2.5f);   

                __m128 Y = _mm_set_ps1((float)y0 / HEIGHT * 2.0f - 1.0f);


                __m128i color =  _mm_setzero_si128();

                get_colour(X, Y, color);

                int * color_int = (int *) &color;

                for (int i = 0; i < 4; i++) {
                    sf::Color sfColor((color_int[i] * 6) % 256, 0, (color_int[i] * 10) % 256);
                    image.setPixel(x0 + i, y0, sfColor);
                }
            }
        }
// ---------------------------------------------------------------------

        texture.update(image);

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}
