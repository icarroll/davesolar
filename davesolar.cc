#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

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

void drawstuff(cairo_t * cr) {
    // 0,0 at center of window and 4.5,4.5 at top right
    cairo_scale(cr, SCREEN_WIDTH/9.0, -SCREEN_HEIGHT/9.0);
    cairo_translate(cr, 4.5, -4.5);

    /*
    // set font
    cairo_select_font_face(cr, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 0.2);

    // "Hello,"
    cairo_text_extents_t te1;
    cairo_text_extents(cr, "Hello,", & te1);

    // "World!"
    cairo_text_extents_t te2;
    cairo_text_extents(cr, "World!", & te2);
    */

    double t = 0.0;   // time in seconds since start
    while (true) {
        // clear screen
        cairo_rectangle(cr, -4.5, -4.5, 9, 9);
        cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
        cairo_fill(cr);

        // Central Fire
        cairo_new_sub_path(cr);
        cairo_arc(cr, 0, 0, 0.1, 0, 2*M_PI);
        cairo_set_source_rgb(cr, 1,1,1);
        cairo_fill(cr);

        // Alles
        cairo_new_sub_path(cr);
        cairo_save(cr);
        cairo_rotate(cr, t * (2*M_PI));
        cairo_arc(cr, 1, 0, 0.01, 0, 2*M_PI);
        cairo_set_source_rgb(cr, 1,1,1);
        cairo_fill(cr);
        cairo_restore(cr);

        // Counter-Alles
        cairo_new_sub_path(cr);
        cairo_save(cr);
        cairo_rotate(cr, t * (2*M_PI));
        cairo_arc(cr, -1, 0, 0.01, 0, 2*M_PI);
        cairo_set_source_rgb(cr, 1,1,1);
        cairo_fill(cr);
        cairo_restore(cr);

        // Terrum
        cairo_new_sub_path(cr);
        cairo_save(cr);
        cairo_rotate(cr, t/8 * (2*M_PI));
        cairo_arc(cr, 2, 0, 0.01, 0, 2*M_PI);
        cairo_set_source_rgb(cr, 1,1,1);
        cairo_fill(cr);
        cairo_restore(cr);

        // Hydrus
        cairo_new_sub_path(cr);
        cairo_save(cr);
        cairo_rotate(cr, t/32 * (2*M_PI));
        cairo_arc(cr, 2.8284, 0, 0.01, 0, 2*M_PI);
        cairo_set_source_rgb(cr, 1,1,1);
        cairo_fill(cr);
        cairo_restore(cr);

        // Zephyr
        cairo_new_sub_path(cr);
        cairo_save(cr);
        cairo_rotate(cr, t/128 * (2*M_PI));
        cairo_arc(cr, 3.4961, 0, 0.01, 0, 2*M_PI);
        cairo_set_source_rgb(cr, 1,1,1);
        cairo_fill(cr);
        cairo_restore(cr);

        // Sola
        cairo_new_sub_path(cr);
        cairo_save(cr);
        cairo_rotate(cr, t/512 * (2*M_PI));
        cairo_arc(cr, 4.03698, 0, 0.01, 0, 2*M_PI);
        cairo_set_source_rgb(cr, 1,1,1);
        cairo_fill(cr);
        cairo_restore(cr);

        /*
        // Hello,
        cpVect pos1 = cpBodyGetPosition(body1);
        cpFloat rot1 = cpBodyGetAngle(body1);

        cairo_save(cr);

        cairo_move_to(cr, pos1.x, pos1.y);
        cairo_scale(cr, 1, -1);
        cairo_rotate(cr, rot1);
        cairo_rel_move_to(cr, -te1.width/2, te1.height/2);
        cairo_text_path(cr, "Hello,");
        cairo_set_source_rgb(cr, 0,0,1);
        cairo_fill_preserve(cr);
        cairo_set_line_width(cr, 0.001);
        cairo_set_source_rgb(cr, 0,0,0);
        cairo_stroke(cr);

        cairo_restore(cr);

        // World!
        cpVect pos2 = cpBodyGetPosition(body2);
        cpFloat rot2 = cpBodyGetAngle(body2);

        cairo_save(cr);

        cairo_move_to(cr, pos2.x, pos2.y);
        cairo_scale(cr, 1, -1);
        cairo_rotate(cr, rot2);
        cairo_rel_move_to(cr, -te2.width/2, te2.height/2);
        cairo_text_path(cr, "World!");
        cairo_set_source_rgb(cr, 0,1,0);
        cairo_fill_preserve(cr);
        cairo_set_line_width(cr, 0.001);
        cairo_set_source_rgb(cr, 0,0,0);
        cairo_stroke(cr);

        cairo_restore(cr);
        */

        /*
        for (cpVect dot : dots) {
            cairo_new_sub_path(cr);
            cairo_arc(cr, dot.x, dot.y, 0.01, 0, 2*M_PI);
        }
        cairo_set_source_rgb(cr, 1,0,0);
        cairo_fill(cr);
        */

        SDL_Event e;
        e.type = BLIT_READY;
        SDL_PushEvent(& e);

        this_thread::sleep_for(chrono::milliseconds(20));
        t += 0.020;
    }
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
    }

    drawthread.detach();

    close();

    return 0;
}
