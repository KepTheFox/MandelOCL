template <typename T>
class Window{
    T x_min, x_max, y_min, y_max, currentZoom;

public:
    Window(T x_min, T x_max, T y_min, T y_max){
        this->x_min = x_min;
        this->x_max = x_max;
        this->y_min = y_min;
        this->y_max = y_max;
        this->currentZoom = 1;
    }

    T size() const {
        return (width() * height());
    }

    T width() const {
        return this->x_max - this->x_min;
    }

    T height() const{
        return this->y_max - this->y_min;
    }

    /*X*/
    T xMin() const{
        return this->x_min;
    }

    T xMin(T x_min){
        this->x_min = x_min;
    }

    T xMax() const{
        return this->x_max;
    }

    T xMax(T x_max){
        this->x_max = x_max;
    }

    /*y*/
    T yMin() const{
        return this->y_min;
    }
    
    T yMin(T y_min){
        this->y_min = y_min;
    }

    T yMax() const{
        return this->y_max;
    }

    T yMax(T y_max){
        this->y_max = y_max;
    }

    T getZoom() const{
        return this->currentZoom;
    }

    void setFrame(T x_min, T x_max, T y_min, T y_max){
        this->x_min = x_min;
        this->x_max = x_max;
        this->y_min = y_min;
        this->y_max = y_max;
    }

    void zoom(double factor){
        this->currentZoom *= factor;
        T x_c = width()/2 + this->x_min;
        T y_c = height()/2 + this->y_min;

        // printf("%f, %f\n", x_c, y_c);

        T x_rad = width()/(2 * factor);
        T y_rad = height()/(2 * factor);

        setFrame(x_c - x_rad, x_c + x_rad, y_c - y_rad, y_c + y_rad);
    }

    void move(double x, double y){
        this->x_min += x;
        this->x_max += x;
        this->y_min += y;
        this->y_max += y;
    }
};