#if 0
 #include <SDL2/SDL.h>
#define WIDTH 640
#define HEIGHT 480

#define SDL_REFR_PERIOD	    100	/*ms*/
static SDL_Window * window;
static SDL_Renderer * renderer;
static SDL_Texture * texture;
static uint32_t tft_fb[MONITOR_HOR_RES * MONITOR_VER_RES];
static volatile bool sdl_inited = false;
static volatile bool sdl_refr_qry = false;
static volatile bool sdl_quit_qry = false;

int quit_filter (void *userdata, SDL_Event * event);
void mouse_handler(SDL_Event *event);

static int sdl_refr(void * param)
{
    (void)param;
	gprintf("#GSDL_refr");
	/*Initialize the SDL*/
    SDL_Init(SDL_INIT_VIDEO);

    SDL_SetEventFilter(quit_filter, NULL);

	window = SDL_CreateWindow("GRuiiK toolkit running on PC TFT simulator",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		MONITOR_HOR_RES, MONITOR_VER_RES, 0);       /*last param. SDL_WINDOW_BORDERLESS to hide borders*/

	renderer = SDL_CreateRenderer(window, -1, 0);
	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, MONITOR_HOR_RES, MONITOR_VER_RES);

	/*Initialize the frame buffer to gray (77 is an empirical value) */
	memset(tft_fb, 77, MONITOR_HOR_RES * MONITOR_VER_RES * sizeof(uint32_t));
	SDL_UpdateTexture(texture, NULL, tft_fb, MONITOR_HOR_RES * sizeof(uint32_t));
	sdl_refr_qry = true;
	sdl_inited = true;

	/*Run until quit event not arrives*/
	while(sdl_quit_qry == false)
		{
		//PR("R")
		/*Refresh handling*/
		if(sdl_refr_qry != false) {
            sdl_refr_qry = false;
            SDL_UpdateTexture(texture, NULL, tft_fb, MONITOR_HOR_RES * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
		PR("R")
		}

	    SDL_Event event;
	    while(SDL_PollEvent(&event)) {
			PR("!")
#if USE_MOUSE != 0
            mouse_handler(&event);
#endif

#if USE_KEYBOARD
            keyboard_handler(&event);
#endif
	    }
	//gprintf("<zz");

		/*Sleep some time*/
		SDL_Delay(SDL_REFR_PERIOD);
			//gprintf("zz>");

	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	exit(0);

	return 0;
}
int quit_filter (void *userdata, SDL_Event * event)
{
    (void)userdata;

	if(event->type == SDL_QUIT) {
		sdl_quit_qry = true;
	}

	return 1;
}



void monitor_init(void)
{
	SDL_CreateThread(sdl_refr, "sdl_refr", NULL);

	while(sdl_inited == false); /*Wait until 'sdl_refr' initializes the SDL*/
	gprintf("#Mmonitor init");
}

void mouse_handler(SDL_Event *event)
{
	switch (event->type) {
        case SDL_MOUSEBUTTONUP:
            if (event->button.button == SDL_BUTTON_LEFT)
                left_button_down = false;
             //gprintf("@k<%g|%g>", last_x, last_y);

            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event->button.button == SDL_BUTTON_LEFT) {
                left_button_down = true;
                last_x = event->motion.x / MAGNIFY;
                last_y = event->motion.y / MAGNIFY;
              //gprintf("@k<%m|%m>", last_x, last_y);
			//std::cerr << "< BD " << last_x << "," << last_y << ">";

            }
            break;
        case SDL_MOUSEMOTION:
            last_x = event->motion.x / MAGNIFY;
            last_y = event->motion.y / MAGNIFY;
			//td::cerr << "<" << last_x << "," << last_y << ">";
            //gprintf("@k<%r|%r>", last_x, last_y);
            break;
        case SDL_FINGERUP:
            if (1)
                left_button_down = false;
             //gprintf("@k<%g|%g>", last_x, last_y);
			//std::cerr << "< FU " << last_x << "," << last_y << ">";

            break;

        case SDL_FINGERDOWN:
            if (1) {
                left_button_down = true;
                last_x = event->tfinger.x / MAGNIFY;
                last_y = event->tfinger.y / MAGNIFY;
              //gprintf("@k<%m|%m>", last_x, last_y);
			//std::cerr << "< FD  " << last_x << "," << last_y << ">";

            }
            break;
        case SDL_FINGERMOTION:
            last_x = event->tfinger.x / MAGNIFY;
            last_y = event->tfinger.y / MAGNIFY;
			//std::cerr << "< FM  " << last_x << "," << last_y << ">";
            //gprintf("@k<%r|%r>", last_x, last_y);
            break;
    }


}
#endif


























