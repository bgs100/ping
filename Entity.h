class Entity {
 public:
    int x, y, w, h;
    double dX, dY;
    Entity() { dX = dY = 0; }
    Entity(int x, int y, int w, int h) {
        Entity();
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
    void draw(SDL_Renderer *renderer) {
        SDL_Rect r = { x, y, w, h };
        SDL_RenderFillRect(renderer, &r);
    }
};
