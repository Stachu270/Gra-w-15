#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>

#ifndef _GRA_H
#define _GRA_H

struct Options
{
	int border;
	int side_len;
	int tiles;
	bool texture_tiles;
	sf::Image image;
	sf::Color color;
};

class Tile : public sf::Drawable
{
private:
	static sf::Font _font;
	static int _count;
protected:
	sf::Text _nr;
	int _id;
	int _pos_x, _pos_y;
	int _size;

	void update_text_pos(int x, int y);
public:
	Tile() {};
	Tile(int pos_x, int pos_y, int size, int id = 0);
	virtual void move_pos(int x, int y) = 0;
	//virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const { this->draw(target, states); }
	virtual void help_on_off() {};
	int & id() { return _id; }

	static void reset() { _count = 1; }
};

class Number_Tile : public Tile
{
private:
	sf::RectangleShape _shp;
public:
	Number_Tile(int pos_x, int pos_y, int size, sf::Color color, int id = 0);
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const { target.draw(_shp, states); target.draw(_nr, states); }
	virtual void move_pos(int x, int y) { _shp.move(sf::Vector2f(float(x), float(y))); _nr.move(sf::Vector2f(float(x), float(y))); }
};

class Image_Tile : public Tile
{
private:
	sf::Sprite _sprt;
	bool _help;
public:
	Image_Tile(int pos_x, int pos_y, int size, sf::Sprite sprite, int id = 0);
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const { target.draw(_sprt, states); if (_help) target.draw(_nr, states); }
	virtual void move_pos(int x, int y) { _sprt.move(sf::Vector2f(float(x), float(y))); _nr.move(sf::Vector2f(float(x), float(y))); }
	virtual void help_on_off() { (_help ^= 0x1); }
};

class Board : public sf::Drawable
{
private:
	Tile ***_board;
	const Options &_opt;
	sf::RenderWindow &_okno;
	sf::Texture _texture_1;
	int _0tile_x, _0tile_y;

	int pomoc(sf::Sprite &spr);
public:
	Board(sf::RenderWindow &wnd, const Options &opt);
	~Board();
	void up(bool show_move);
	void down(bool show_move);
	void left(bool show_move);
	void right(bool show_move);
	void shuffle(int count);
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	bool is_good() const;
	void help();
};

class Game
{
private:
	Board *_board;
	Options _options;
	sf::RenderWindow _okno;

	std::vector<std::string> image_files_list();
	int main_menu(sf::RenderWindow &window);
	std::string file_menu(sf::RenderWindow &window);
public:
	Game();
	~Game() { delete _board; }
	void init();
	void change_options(const Options &opt) { _options = opt; }
	void play();
	void win_message();
};


class Label : public sf::Drawable
{
private:
	static sf::Font _font;

	sf::Text _txt;
	sf::RectangleShape _box;
public:
	Label(int x, int y, const std::string &String, sf::Color Text_Color, std::size_t Text_Size, sf::Color Background_color, std::size_t Outline_Thickness, sf::Color Outline_Color );
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const { target.draw(_box, states); target.draw(_txt, states); }
};

class Button : public sf::Drawable
{
private:
	static sf::Font _font;

	sf::IntRect _wsp;
	sf::Text _txt;
	sf::RectangleShape _shp_normal;
	sf::RectangleShape _shp_clicked;
	bool clicked;
public:
	Button(int x, int y, int w, int h, const std::string &str);
	Button(int x, int y, const std::string &str);
	sf::Vector2f getPosition() const { return sf::Vector2f(float(_wsp.width), float(_wsp.top)); }
	void setPosition(const sf::Vector2f &vec) { _wsp.top = vec.y; _wsp.left = vec.x; _txt.setPosition(vec.x, vec.y); _shp_normal.setPosition(vec.x, vec.y); _shp_clicked.setPosition(vec.x, vec.y); }
	void setOrigin(const sf::Vector2f &vec) { _txt.setOrigin(vec.x, vec.y); _shp_normal.setOrigin(vec.x, vec.y); _shp_clicked.setOrigin(vec.x, vec.y); }
	void move(int x, int y) { _wsp.top += y; _wsp.left += x; _txt.move(float(x), float(y)); _shp_normal.move(float(x), float(y)); _shp_clicked.move(float(x), float(y)); }
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const { (clicked) ? target.draw(_shp_clicked, states) : target.draw(_shp_normal, states); target.draw(_txt, states); }
	void show(sf::RenderWindow &wnd) { (clicked) ? wnd.draw(_shp_clicked) : wnd.draw(_shp_normal); wnd.draw(_txt); }
	void click() { clicked ^= 0x1; }
};

class Controls_Box : public sf::Drawable
{
private:
	static sf::Font _font;

	std::vector<sf::Drawable *> _elem_tab;
	sf::IntRect _wsp;
	sf::RectangleShape _box;
	sf::RectangleShape _box_active;
	//sf::Text _txt;
	//Button _txt;
	Label _txt;
	bool _active;
public:
	Controls_Box(int x, int y, int w, int h, const std::string &str);
	void add(sf::Drawable &ref) { _elem_tab.push_back(&ref); }
	void add(Button &ref) { ref.move(_wsp.left, _wsp.top); _elem_tab.push_back(&ref); }
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	const sf::Vector2f &getOrigin() const { return _box.getOrigin(); }
	void active() { _active ^= 0x1; }
};

#endif
