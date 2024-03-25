#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>

const int N_MAX = 256;
const float RADIUS = 10.0f;
const int WIDTH = 800;
const int HEIGHT = 600;

inline void get_colour(float * X0, float * Y0, int * color) {

    float X[4] = {X0[0], X0[1], X0[2], X0[3]};
    float Y[4] = {Y0[0], Y0[1], Y0[2], Y0[3]};
    float XX[4] = {0};
    float YY[4] = {0};
    float XY[4] = {0};

    for (int n = 0; n < N_MAX; n++) {

        for (int j = 0; j < 4; j++) {
            XX[j] = X[j] * X[j];
            YY[j] = Y[j] * Y[j];
            XY[j] = Y[j] * X[j];
        }

        int cmp[4] = {0};
        for (int j = 0; j < 4; j++) {
            if (XX[j] + YY[j] <= RADIUS) {
                cmp[j] = 1;
            }
        }

        int mask = 0;
        for (int j = 0; j < 4; j++) {
            mask |= (cmp[j] << j);
        }

        if (!mask) break;

        for (int j = 0; j < 4; j++) {
            color[j] += cmp[j];
        }

        for (int j = 0; j < 4; j++) {
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

    sf::Font font;
    sf::Text fpsText("", font, 20);
    fpsText.setColor(sf::Color::White);
    fpsText.setPosition(10, 10);

    sf::Clock gameClock; // Переименовано из clock в gameClock
    int frames = 0;
    int n_of_frames = 0;
    float fps_sum = 0;

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

                float X[4] = {0};
                float Y[4] = {0};

// ------------------------------------------------------------------------ заполнить массив
                float Y0_const = (float)y0 / HEIGHT * 2.0f - 1.0f;
                for (int i = 0; i < 4; i++) {
                    X[i] = (float)(x0 + i) / WIDTH * 3.5f - 2.5f;       // dx --------------------------!!!!!!!!!!!!!!!!1
                    Y[i] = Y0_const;
                }
// ------------------------------------------------------------------------

                int color[4] = {0};
                get_colour(X, Y, color);

                for (int i = 0; i < 4; i++) {
                    sf::Color sfColor((color[i] * 6) % 256, 0, (color[i] * 10) % 256);
                    image.setPixel(x0 + i, y0, sfColor);
                }
            }
        }
// ---------------------------------------------------------------------

        texture.update(image);

        window.clear();
        window.draw(sprite);

        frames++;
        n_of_frames++;

        if (gameClock.getElapsedTime().asSeconds() >= 1.0) {
            float fps = frames / gameClock.getElapsedTime().asSeconds();

            fps_sum += fps;
            if (n_of_frames == 10) {
                printf("average fps is: %f\n", fps_sum / 10);
                gameClock.restart();
                fps_sum = 0;
                n_of_frames = 0;
            }

            gameClock.restart();
            frames = 0;
        }
        window.display();
    }

    return 0;
}

