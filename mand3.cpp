#include <SFML/Graphics.hpp>
#include <cmath>
#include <immintrin.h>
  

const int MAX_ITERATIONS = 256;
const __m128 RADIUS = _mm_set_ps1(100.f);
const int WIDTH = 800;
const int HEIGHT = 600;

//#define COUNT_FPS

#ifdef COUNT_FPS

    int countity_of_calc = 0;
    unsigned long long sum_of_fps = 0;
    const int base_number = 5;
#endif

inline void get_colour(__m128 X0, __m128 Y0, volatile  __m128 *  color) {

    __m128 X =  _mm_set_ps1(0); // X = {0, 0, 0, 0}
    __m128 Y =  _mm_set_ps1(0); // Y = {0, 0, 0, 0}

    _mm_storeu_si128((__m128i*)&X, (__m128i)X0); // X = X0
    _mm_storeu_si128((__m128i*)&Y, (__m128i)Y0); // Y = Y0

    for (int n = 0; n < MAX_ITERATIONS; n++) {

        __m128 XX = _mm_mul_ps(X,X); // XX == X*X
        __m128 YY = _mm_mul_ps(Y,Y);
        __m128 XY = _mm_mul_ps(X,Y);
        __m128 R2 = _mm_add_ps(YY,XX);

        __m128 cmp = _mm_cmple_ps(R2, RADIUS); // compare R2[i] and Radius[i]
                                                    // a <= b   =>  -1 || 0 = cmp[i]

        int mask = _mm_movemask_ps((__m128)cmp); // 0000|4321 where 1234 are floats from cmp

        if (!mask) break;

        *color =  _mm_sub_epi32(*color, (__m128i)cmp); // color[i] = color[i] - (-1 || 0)

        _mm_storeu_si128((__m128i*)&X, (__m128i)_mm_add_ps(_mm_sub_ps(XX,YY), X0));
        _mm_storeu_si128((__m128i*)&Y, (__m128i)_mm_add_ps(_mm_add_ps(XY,XY), Y0));
    }
}


inline void draw_vector(sf::Image* image, sf::Color pixel_array[600][800], int * color_int, int x0, int y0) {

    for (int i = 0; i < 4; i++) {

        sf::Color sfColor((color_int[i] * 6) % 256, 0, (color_int[i] * 10) % 256);
        (*image).setPixel(x0 + i, y0, sfColor);
    }
}

#ifdef COUNT_FPS
inline void calculate_fps(unsigned long long start, unsigned long long end) {

    countity_of_calc++;
    unsigned long long dif = end - start;
    sum_of_fps += dif;

    if (countity_of_calc == base_number) {

        printf("average ticks for %d times: %llu\n\n", base_number, sum_of_fps/base_number);
        sum_of_fps = 0;
        countity_of_calc = 0;
    }
}
#endif

void mandelbrot(sf::Image* image, sf::Color pixel_array[600][800]){

    #ifdef COUNT_FPS
        unsigned long long start = __rdtsc();
    #endif

    for (int y0 = 0; y0 < HEIGHT; y0++) {
        for (int x0 = 0; x0 < WIDTH; x0+=4) {

            __m128 c3210 = _mm_set_ps(3,2,1,0);
            __m128 WIDTH_VECTOR = _mm_set_ps1(WIDTH);
            __m128 c35 = _mm_set_ps1(3.5);
            __m128 c25 = _mm_set_ps1(2.5);

            __m128 X = _mm_set_ps1(x0);                //
            X = _mm_add_ps(X, c3210);               // (float)(x0 + {3, 2, 1, 0}) / WIDTH * 3.5f - 2.5f,
            X = _mm_div_ps(X, WIDTH_VECTOR);        //
            X = _mm_mul_ps(X, c35);                 //
            X = _mm_sub_ps(X, c25);                 //


            __m128 Y = _mm_set_ps1((float)y0 / HEIGHT * 2.0f - 1.0f);


            volatile __m128 color =  _mm_setzero_si128();

            get_colour(X, Y, &color);

            int * color_int = (int *) &color;


            #ifndef COUNT_FPS
                draw_vector(image, pixel_array, color_int, x0, y0);
            #endif

        }
    }

    #ifdef COUNT_FPS
        unsigned long long end = __rdtsc();
        calculate_fps(start, end);
    #endif
}



int main() {

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot Set");

    sf::Image image;
    image.create(WIDTH, HEIGHT, sf::Color::Black);

    sf::Texture texture;
    texture.create(WIDTH, HEIGHT);
    sf::Sprite sprite(texture);

    sf::Color pixel_array[600][800];

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        mandelbrot(&image, pixel_array);

        texture.update(image);
        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}


// TODO:
        // &&&&&&&&&&&&&&&&&&&&&&
        // мб сдвиги и scale