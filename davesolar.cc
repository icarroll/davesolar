#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <chipmunk.h>

extern "C" {
#include <SDL.h>
#include <cairo.h>
}

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

char WINDOW_NAME[] = "David's Solar System";
SDL_Window * gWindow = NULL;

void die(string message) {
    cout << message << endl;
    exit(1);
}

void init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) die("SDL");
    if (! SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) die("texture");

    gWindow = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                               SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if (gWindow == NULL) die("window");
}

void close()
{
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    SDL_Quit();
}

int BLIT_READY;

/*
Alles (and its counter Alles which is 180 degrees away in the orbit) have a 1 unit radius and orbit in one day
Terrum has a radius of 2 and orbits weekly (once for every 8 Alles rotations)
Hydrus has a radius of 2.8284 (probably safe to call it 2.8 or 2.83 if you want) and orbits monthly (once for every 4 orbits of Terrum or 32 Alles orbits)
Zephyr has a radius of 3.4961 (probably safe to call it 3.5) and has one orbit for every 4 Hydrus orbits or 128 Alles orbits)
and Sola has a radius of 4.03698 (probably safe to call it 4.0 or 4.04) with one orbit for every four Zephyr orbits or 512 Alles orbits
those radii are of course orbital radii
*/

double TIME_STEP_STEP = 0.020;   // amount to change time_step by
double time_step = 0.020;   // current time step in seconds
double current_time = 0.0;   // time in seconds (Alles-days) since start

// these phases are all in radians
enum planet_t    {ALLES, TERRUM, HYDRUS, ZEPHYR, SOLA};
double phase[] = {0.0,   0.0,    0.0,    0.0,    0.0};

void drawstuff(cairo_t * cr) {
    // 0,0 at center of window and 4.5,4.5 at top right
    cairo_scale(cr, SCREEN_WIDTH/9.0, -SCREEN_HEIGHT/9.0);
    cairo_translate(cr, 4.5, -4.5);

    while (true) {
        // clear screen
        cairo_rectangle(cr, -4.5, -4.5, 9, 9);
        cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
        cairo_fill(cr);

        // Central Fire
        cairo_new_sub_path(cr);
        double radius = 0.1;
        double angle = M_PI/2;
        cairo_move_to(cr, radius*cos(angle), radius*sin(angle));
        for (int n=0 ; n<5 ; n+=1) {
            angle += 2*M_PI/(360.0/144.0);
            cairo_line_to(cr, radius*cos(angle), radius*sin(angle));
        }
        cairo_set_source_rgb(cr, 0.5,0,1);
        cairo_fill(cr);

        // Alles
        cairo_new_sub_path(cr);
        cairo_save(cr);
        cairo_rotate(cr, phase[ALLES] + current_time * (2*M_PI));
        cairo_arc(cr, 1, 0, 0.01, 0, 2*M_PI);
        cairo_set_source_rgb(cr, 1,1,1);
        cairo_fill(cr);
        cairo_restore(cr);

        // Counter-Alles
        cairo_new_sub_path(cr);
        cairo_save(cr);
        cairo_rotate(cr, phase[ALLES] + current_time * (2*M_PI));
        cairo_arc(cr, -1, 0, 0.01, 0, 2*M_PI);
        cairo_set_source_rgb(cr, 1,1,1);
        cairo_fill(cr);
        cairo_restore(cr);

        // Terrum
        cairo_new_sub_path(cr);
        cairo_save(cr);
        cairo_rotate(cr, phase[TERRUM] + current_time/8 * (2*M_PI));
        cairo_arc(cr, 2, 0, 0.01, 0, 2*M_PI);
        cairo_set_source_rgb(cr, 1,1,1);
        cairo_fill(cr);
        cairo_restore(cr);

        // Hydrus
        cairo_new_sub_path(cr);
        cairo_save(cr);
        cairo_rotate(cr, phase[HYDRUS] + current_time/32 * (2*M_PI));
        cairo_arc(cr, 2.8284, 0, 0.01, 0, 2*M_PI);
        cairo_set_source_rgb(cr, 1,1,1);
        cairo_fill(cr);
        cairo_restore(cr);

        // Zephyr
        cairo_new_sub_path(cr);
        cairo_save(cr);
        cairo_rotate(cr, phase[ZEPHYR] + current_time/128 * (2*M_PI));
        cairo_arc(cr, 3.4961, 0, 0.01, 0, 2*M_PI);
        cairo_set_source_rgb(cr, 1,1,1);
        cairo_fill(cr);
        cairo_restore(cr);

        // Sola
        cairo_new_sub_path(cr);
        cairo_save(cr);
        cairo_rotate(cr, phase[SOLA] + current_time/512 * (2*M_PI));
        cairo_arc(cr, 4.03698, 0, 0.01, 0, 2*M_PI);
        cairo_set_source_rgb(cr, 1,1,1);
        cairo_fill(cr);
        cairo_restore(cr);

        SDL_Event e;
        e.type = BLIT_READY;
        SDL_PushEvent(& e);

        this_thread::sleep_for(chrono::milliseconds(20));
        current_time += time_step;
    }
}

double avg(double a, double b) {
    return (a + b) / 2.0;
}

int main(int nargs, char * args[])
{
    init();

    SDL_Surface * sdlsurf = SDL_CreateRGBSurface(
        0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
        0x00FF0000, // red
        0x0000FF00, // green
        0x000000FF, // blue
        0); // alpha

    //TODO make sure sdlsurf is locked or doesn't need locking

    cairo_surface_t * csurf = cairo_image_surface_create_for_data(
        (unsigned char *) sdlsurf->pixels,
        CAIRO_FORMAT_RGB24,
        sdlsurf->w,
        sdlsurf->h,
        sdlsurf->pitch);

    cairo_t * cr = cairo_create(csurf);

    BLIT_READY = SDL_RegisterEvents(1);
    thread drawthread(drawstuff, cr);

    SDL_Surface * wsurf = SDL_GetWindowSurface(gWindow);

    bool drag;
    planet_t drag_planet;
    double oldangle;
    double startphase;

    bool done = false;
    while (! done)
    {
        SDL_Event e;
        SDL_WaitEvent(& e); //TODO check for error

        if (e.type == SDL_QUIT) done = true;
        else if (e.type == BLIT_READY) {
            SDL_BlitSurface(sdlsurf, NULL, wsurf, NULL);
            SDL_UpdateWindowSurface(gWindow);
        }
        else if (e.type == SDL_KEYDOWN) {
            SDL_Keycode k = e.key.keysym.sym;
            if (k == SDLK_MINUS) {
                time_step -= TIME_STEP_STEP;
            }
            else if (k == SDLK_PLUS || k == SDLK_EQUALS) {
                time_step += TIME_STEP_STEP;
            }
            else if (k == SDLK_SPACE) {
                current_time += TIME_STEP_STEP;
            }
            else if (k == SDLK_q) done = true;
        }
        else if (e.type == SDL_MOUSEBUTTONDOWN) {
            double x = e.button.x;
            double y = e.button.y;
            cairo_device_to_user(cr, & x, & y);

            double len = cpvlength(cpv(x,y));
            if (len < avg(0,1)) continue;
            else if (len < avg(1,2)) drag_planet = ALLES;
            else if (len < avg(2,2.8284)) drag_planet = TERRUM;
            else if (len < avg(2.8284,3.4961)) drag_planet = HYDRUS;
            else if (len < avg(3.4961,4.03698)) drag_planet = ZEPHYR;
            else if (len < avg(4.03698, 4.5)) drag_planet = SOLA;
            else continue;

            oldangle = cpvtoangle(cpv(x,y));
            startphase = phase[drag_planet];
            drag = true;
        }
        else if (drag && e.type == SDL_MOUSEMOTION) {
            double x = e.motion.x;
            double y = e.motion.y;
            cairo_device_to_user(cr, & x, & y);

            double newangle = cpvtoangle(cpv(x,y));
            phase[drag_planet] = startphase + newangle - oldangle;
        }
        else if (e.type == SDL_MOUSEBUTTONUP) {
            drag = false;
        }
    }

    drawthread.detach();

    close();

    return 0;
}
