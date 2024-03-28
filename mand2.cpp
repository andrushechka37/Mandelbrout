#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>

const int N_MAX = 256;
const float RADIUS = 10.0f;
const int WIDTH = 800;
const int HEIGHT = 600;
const int LEN_OF_VECTOR = 4;

#define COUNT_FPS

#ifdef COUNT_FPS
    int countity_of_calc = 0;
    unsigned long long sum_of_fps = 0;
    const int base_number = 5;
#endif

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

inline void get_colour(float * X0, float * Y0, volatile int * color) {

    float  X[LEN_OF_VECTOR] = {X0[0], X0[1], X0[2], X0[3]};
    float  Y[LEN_OF_VECTOR] = {Y0[0], Y0[1], Y0[2], Y0[3]};
    float XX[LEN_OF_VECTOR] = {0};
    float YY[LEN_OF_VECTOR] = {0};
    float XY[LEN_OF_VECTOR] = {0};

    for (int n = 0; n < N_MAX; n++) {

        for (int j = 0; j < LEN_OF_VECTOR; j++) {
            XX[j] = X[j] * X[j];
            YY[j] = Y[j] * Y[j];
            XY[j] = Y[j] * X[j];
        }

        int cmp[LEN_OF_VECTOR] = {0};
        for (int j = 0; j < LEN_OF_VECTOR; j++) {
            if (XX[j] + YY[j] <= RADIUS) {
                cmp[j] = 1;
            }
        }

        int mask = 0;
        for (int j = 0; j < LEN_OF_VECTOR; j++) {
            mask |= (cmp[j] << j);
        }

        if (!mask) break;

        for (int j = 0; j < LEN_OF_VECTOR; j++) {
            color[j] += cmp[j];
            X[j] = XX[j] - YY[j] + X0[j];
            Y[j] = XY[j] + XY[j] + Y0[j];
        }
    }
}

int main() {

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot Set");

    sf::Image image;
    image.create(WIDTH, HEIGHT, sf::Color::Black);

    sf::Texture texture;
    texture.create(WIDTH, HEIGHT);
    sf::Sprite sprite(texture);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

// ---------------------------------------------------------------------

        #ifdef COUNT_FPS
            unsigned long long start = __rdtsc();
        #endif

        for (int y0 = 0; y0 < HEIGHT; y0++) {
            for (int x0 = 0; x0 < WIDTH; x0+=4) {

                float X[LEN_OF_VECTOR] = {0};
                float Y[LEN_OF_VECTOR] = {0};

                float Y0_const = (float)y0 / HEIGHT * 2.0f - 1.0f;
                for (int i = 0; i < LEN_OF_VECTOR; i++) {
                    X[i] = (float)(x0 + i) / WIDTH * 3.5f - 2.5f;    
                    Y[i] = Y0_const;
                }
// ------------------------------------------------------------------------

                volatile int color[LEN_OF_VECTOR] = {0};
                get_colour(X, Y, color);

                #ifndef COUNT_FPS
                    for (int i = 0; i < LEN_OF_VECTOR; i++) {
                        sf::Color sfColor((color[i] * 6) % 256, 0, (color[i] * 10) % 256);
                        image.setPixel(x0 + i, y0, sfColor);
                    }
                #endif
            }
        }

        #ifdef COUNT_FPS
            unsigned long long end = __rdtsc();
            calculate_fps(start, end);
        #endif
// ---------------------------------------------------------------------

        texture.update(image);

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}

