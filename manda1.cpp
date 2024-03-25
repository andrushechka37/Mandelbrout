#include <SFML/Graphics.hpp>

const int N_MAX          = 256;
const float RADIUS       = 10.0f;
const int WIDTH          = 800;
const int HEIGHT         = 600;


int get_colour(float x0, float y0) {

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

        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {

                float xx = (float)x / WIDTH * 3.5f - 2.5f;
                float yy = (float)y / HEIGHT * 2.0f - 1.0f;

                int color = get_colour(xx, yy);
                sf::Color sfColor((color * 6) % 256, 0, (color * 10) % 256);
                image.setPixel(x, y, sfColor);
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
