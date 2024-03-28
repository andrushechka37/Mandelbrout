#include <SFML/Graphics.hpp>

const int N_MAX          = 256;
const float RADIUS       = 10.0f;
const int WIDTH          = 800;
const int HEIGHT         = 600;

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


inline int get_colour(float x0, float y0) {

    float x  = 0.0f;
    float y  = 0.0f;
    float xx = 0.0f;
    float yy = 0.0f;
    float xy = 0;

    int i = 0;

    while (xx + yy <= RADIUS && i < N_MAX) {
        xx = x*x;
        yy = y*y;
        xy = x*y;
        
        y = 2*xy + y0;
        x = xx - yy + x0;
        i++;
    }

    if (i == N_MAX) {
        return 0;       
    } else {
        return i;
    }
}

int main() {


    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot Set"); // make window

    sf::Image image;                                                     // картинка на которой пикселями рисуем
    image.create(WIDTH, HEIGHT, sf::Color::Black);  //

    sf::Texture texture;                              //
    texture.create(WIDTH, HEIGHT);      // smth
    sf::Sprite sprite(texture);                       //

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


        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {

                float xx = (float)x / WIDTH * 3.5f - 2.5f;
                float yy = (float)y / HEIGHT * 2.0f - 1.0f;

                volatile int color = get_colour(xx, yy);

                #ifndef COUNT_FPS
                    sf::Color sfColor((color * 6) % 256, 0, (color * 10) % 256);
                    image.setPixel(x, y, sfColor);
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
