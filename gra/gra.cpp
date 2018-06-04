#include "gra.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

// FUNKCJE POMOCNICZE
int text_width(const sf::Text txt) { return static_cast<int>(txt.getLocalBounds().width); }
int text_top(const sf::Text txt) { return static_cast<int>(txt.getLocalBounds().top); }
int text_left(const sf::Text txt) { return static_cast<int>(txt.getLocalBounds().left); }
int text_height(const sf::Text txt) { return static_cast<int>(txt.getLocalBounds().height); }


// KLASA TILE
int Tile::_count = 1;
sf::Font Tile::_font;

Tile::Tile(int pos_x, int pos_y, int size, int id) : _pos_x(pos_x), _pos_y(pos_y), _size(size), _id((id) ? id : _count++)
{
	_font.loadFromFile("arial.ttf");
	char tmp[10];
	_nr.setFont(_font);
	_nr.setString(itoa(_id, tmp, 10));
}

// x = -1, 0, 1; y = -1, 0, 1
void Tile::update_text_pos(int x, int y)
{
	int half_h = (_size - text_height(_nr)) / 2 - 1;
	int half_w = (_size - text_width(_nr)) / 2 - 1;

	_nr.setPosition(
		sf::Vector2f(
		float(_pos_x + (_size - text_width(_nr)) / 2 - text_left(_nr) + x * half_w),
		float(_pos_y + (_size - text_height(_nr)) / 2 - text_top(_nr) + y * half_h)
		)
		);
}


// KLASA NUMBER_TILE
Number_Tile::Number_Tile(int pos_x, int pos_y, int size, sf::Color color, int id) : Tile(pos_x, pos_y, size, id), _shp(sf::Vector2f(float(size), float(size)))
{
	_shp.setPosition(sf::Vector2f(float(pos_x), float(pos_y)));
	_shp.setFillColor(color);
	_nr.setCharacterSize(unsigned(0.6 * size));

	update_text_pos(0, 0);
}


// KLASA IMAGE_TILE
Image_Tile::Image_Tile(int pos_x, int pos_y, int size, sf::Sprite sprite, int id) : Tile(pos_x, pos_y, size, id), _sprt(sprite), _help(false)
{
	_nr.setCharacterSize(15);
	_sprt.setPosition(sf::Vector2f(float(pos_x), float(pos_y)));
	
	update_text_pos(-1, -1);
}


// KLASA BOARD
Board::Board(sf::RenderWindow &wnd, const Options &opt) : _okno(wnd), _opt(opt), _0tile_x(opt.tiles - 1), _0tile_y(opt.tiles - 1)// _size(opt.tiles), _pos_x(opt.pos_x), _pos_y(opt.pos_y), _border(opt.border), _side_len(opt.side_len)
{
	_board = new Tile **[_opt.tiles];
	for (int i = 0; i < _opt.tiles; i++)
		_board[i] = new Tile *[_opt.tiles];


	int s_b = _opt.side_len + _opt.border;
	if (_opt.texture_tiles)
	{
		sf::Sprite spr;
		int len = pomoc(spr);

		for (int i = 0; i < _opt.tiles; i++)
		{
			for (int j = 0; j < _opt.tiles; j++)
			{
				spr.setTextureRect(sf::IntRect(j * len, i * len, len, len));
				_board[i][j] = new Image_Tile(_opt.border + j * s_b, _opt.border + i * s_b, _opt.side_len, spr);
			}
		}
	}
	else
	{
		for (int i = 0; i < _opt.tiles; i++)
			for (int j = 0; j < _opt.tiles; j++)
				_board[i][j] = new Number_Tile(_opt.border + j * s_b, _opt.border + i * s_b, _opt.side_len, _opt.color);
	}
	
	Tile::reset();
}

Board::~Board()
{
	for (int i = 0; i < _opt.tiles; i++)
	{
		for (int j = 0; j < _opt.tiles; j++)
		{
			delete _board[i][j];
		}
		delete[] _board[i];
	}
	delete[] _board;
}

int Board::pomoc(sf::Sprite &spr)
{
	//sf::Sprite spr;
	int min_size = (_opt.image.getSize().x > _opt.image.getSize().y) ? _opt.image.getSize().y : _opt.image.getSize().x;
	int len = min_size / _opt.tiles;
	
	min_size = len * _opt.tiles;

	float factor = float(_opt.side_len) / len;

	_texture_1.loadFromImage(_opt.image, sf::IntRect(0, 0, min_size, min_size));
	spr.setTexture(_texture_1);
	spr.setScale(factor, factor);

	return len;
}

void Board::up(bool show_move)
{
	if (_0tile_y == _opt.tiles - 1)
		return;
	
	Tile *tmp = _board[_0tile_y][_0tile_x];
	_board[_0tile_y][_0tile_x] = _board[_0tile_y + 1][_0tile_x];
	_board[++_0tile_y][_0tile_x] = tmp;
	//_board[_0tile_y][_0tile_x].move_pos(0, _opt.side_len + _opt.border);

	if (!show_move)
	{
		_board[_0tile_y - 1][_0tile_x]->move_pos(0, -_opt.side_len - _opt.border);
		return;
	}

	sf::Clock clck;
	int s_b = _opt.side_len + _opt.border;
	int jmp = s_b / 15;
	int rest = s_b % 15;
	for (int i = 0; i <= 15; i++)
	{
		clck.restart();
		while (clck.getElapsedTime().asMilliseconds() < 5)
			continue;

		_board[_0tile_y - 1][_0tile_x]->move_pos(0, -((i == 15) ? rest : jmp));
		_okno.clear();
		_okno.draw(*this);
		_okno.display();
	}
}

void Board::down(bool show_move)
{
	if (_0tile_y == 0)
		return;

	Tile *tmp = _board[_0tile_y][_0tile_x];
	_board[_0tile_y][_0tile_x] = _board[_0tile_y - 1][_0tile_x];
	_board[--_0tile_y][_0tile_x] = tmp;
	//_board[_0tile_y][_0tile_x].move_pos(0, -_side_len - _border);

	if (!show_move)
	{
		_board[_0tile_y + 1][_0tile_x]->move_pos(0, _opt.side_len + _opt.border);
		return;
	}

	sf::Clock clck;
	int s_b = _opt.side_len + _opt.border;
	int jmp = s_b / 15;
	int rest = s_b % 15;
	for (int i = 0; i <= 15; i++)
	{
		clck.restart();
		while (clck.getElapsedTime().asMilliseconds() < 5)
			continue;

		_board[_0tile_y + 1][_0tile_x]->move_pos(0, ((i == 15) ? rest : jmp));
		_okno.clear();
		_okno.draw(*this);
		_okno.display();
	}
}

void Board::left(bool show_move)
{
	if (_0tile_x == _opt.tiles - 1)
		return;

	Tile *tmp = _board[_0tile_y][_0tile_x];
	_board[_0tile_y][_0tile_x] = _board[_0tile_y][_0tile_x + 1];
	_board[_0tile_y][++_0tile_x] = tmp;
	//_board[_0tile_y][_0tile_x].move_pos(_side_len + _border, 0);

	if (!show_move)
	{
		_board[_0tile_y][_0tile_x - 1]->move_pos(-_opt.side_len - _opt.border, 0);
		return;
	}

	sf::Clock clck;
	int s_b = _opt.side_len + _opt.border;
	int jmp = s_b / 15;
	int rest = s_b % 15;
	for (int i = 0; i <= 15; i++)
	{
		clck.restart();
		while (clck.getElapsedTime().asMilliseconds() < 5)
			continue;

		_board[_0tile_y][_0tile_x - 1]->move_pos(-((i == 15) ? rest : jmp), 0);
		_okno.clear();
		_okno.draw(*this);
		_okno.display();
	}
}

void Board::right(bool show_move)
{
	if (_0tile_x == 0)
		return;

	Tile *tmp = _board[_0tile_y][_0tile_x];
	_board[_0tile_y][_0tile_x] = _board[_0tile_y][_0tile_x - 1];
	_board[_0tile_y][--_0tile_x] = tmp;
	//_board[_0tile_y][_0tile_x].move_pos(-_side_len - _border, 0);

	if (!show_move)
	{
		_board[_0tile_y][_0tile_x + 1]->move_pos(_opt.side_len + _opt.border, 0);
		return;
	}

	sf::Clock clck;
	int s_b = _opt.side_len + _opt.border;
	int jmp = s_b / 15;
	int rest = s_b % 15;
	for (int i = 0; i <= 15; i++)
	{
		clck.restart();
		while (clck.getElapsedTime().asMilliseconds() < 5)
			continue;

		_board[_0tile_y][_0tile_x + 1]->move_pos(((i == 15) ? rest : jmp), 0);
		_okno.clear();
		_okno.draw(*this);
		_okno.display();
	}

}

void Board::shuffle(int count)
{
	srand(unsigned(time(0)));

	for (int i = 0; i < count; i++)
	{
		switch (rand() % 4)
		{
		case 0:
			up(false);
			break;
		case 1:
			down(false);
			break;
		case 2:
			left(false);
			break;
		case 3:
			right(false);
			break;
		}
	}
}

void Board::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	for (int i = 0; i < _opt.tiles; i++)
		for (int j = 0; j < _opt.tiles; j++)
		{
		if (i == _0tile_y && j == _0tile_x)
			continue;
		target.draw(*(_board[i][j]), states);
		}
}

bool Board::is_good() const
{
	int licz = 1;
	for (int i = 0; i < _opt.tiles; i++)
		for (int j = 0; j < _opt.tiles; j++)
			if (_board[i][j]->id() != licz++)
				return false;
	return true;
}

void Board::help()
{
	for (int i = 0; i < _opt.tiles; i++)
		for (int j = 0; j < _opt.tiles; j++)
			_board[i][j]->help_on_off();
}


// KLASA GAME
void Game::win_message()
{	
	sf::RectangleShape end(sf::Vector2f(_okno.getSize()));
	end.setFillColor(sf::Color(180, 180, 180, 190));

	Label napis(0, 0, "Wygrales\nWygralas\n+wcisnij\n+Enter", sf::Color::Black, 20, sf::Color::Transparent, 0, sf::Color::Transparent);

	_okno.draw(end);
	_okno.draw(napis);
}

Game::Game() : _okno(sf::VideoMode(0, 0), "")
{	
	_okno.setKeyRepeatEnabled(false);
	_okno.setPosition(sf::Vector2i(300, 0));
	init();
}

void Game::init()
{
	while (1)
	{
		_options.texture_tiles = main_menu(_okno);

		if (_options.texture_tiles)
		{
			std::string nazwa = file_menu(_okno);
			
			if (nazwa == "")
				continue;
			_options.image.loadFromFile(nazwa);
			_options.border = 0;

			std::cout << "Liczba kafelkow (polecam 4): ";
			int x;
			std::cin >> x;
			_options.tiles = x;

			std::cout << "Szerokosc kafelkow (polecam 150): ";
			int y;
			std::cin >> y;
			_options.side_len = y;
		}
		else
		{
			_options.border = 2;

			std::cout << "Liczba kafelkow (polecam 4): ";
			int x;
			std::cin >> x;
			_options.tiles = x;

			std::cout << "Szerokosc kafelkow (polecam 50): ";
			int y;
			std::cin >> y;

			_options.side_len = y;
			_options.color = sf::Color::Blue;
		}
		break;
	}
	_okno.setSize(sf::Vector2u((_options.tiles + 1) * _options.border + _options.tiles * _options.side_len,
								(_options.tiles + 1) * _options.border + _options.tiles * _options.side_len));
	sf::View view(sf::FloatRect(0.f, 0.f, float((_options.tiles + 1) * _options.border + _options.tiles * _options.side_len), float((_options.tiles + 1) * _options.border + _options.tiles * _options.side_len)));
	_okno.setView(view);
	_okno.setTitle("GRA");

	_board = new Board(_okno, _options);
}

void Game::play()
{	
	_board->shuffle(1000);
	bool win = false;
	sf::Text text;
	sf::Font font;
	font.loadFromFile("arial.ttf");

	//_board.help();

	while (_okno.isOpen())
	{
		sf::Event event;
		while (_okno.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				_okno.close();
				break;
			case sf::Event::KeyPressed:
				if (!win && event.key.code == sf::Keyboard::Up)
				{
					_board->up(true);
					if (_board->is_good())
						win = true;
				}
				else if (!win && event.key.code == sf::Keyboard::Right)
				{
					_board->right(true);
					if (_board->is_good())
						win = true;
				}
				else if (!win && event.key.code == sf::Keyboard::Down)
				{
					_board->down(true);
					if (_board->is_good())
						win = true;
				}
				else if (!win && event.key.code == sf::Keyboard::Left)
				{
					_board->left(true);
					if (_board->is_good())
						win = true;
				}
				else if (event.key.code == sf::Keyboard::Return)
				{
					_board->help();
					if (win)
						_okno.close();
				}
				else if (event.key.code == sf::Keyboard::Escape)
				{
					_okno.close();
				}
				break;
			}
			
		}
		
		

		sf::RectangleShape box;
		if (win)
		{
			//win_message();
			
			/*text.setFont(font);
			text.setCharacterSize(40);
			text.setColor(sf::Color::Red);
			text.setString("Wygrales");
			text.setPosition(0, 0);

			box.setSize(sf::Vector2f(170, 50));
			box.setFillColor(sf::Color::White);*/

			//box.setSize(sf::Vector2f(_okno.getSize()));
			//box.setFillColor(sf::Color(180, 180, 180, 190));
		}

		_okno.clear();
		_okno.draw(*_board);
		//_board->show(_okno);
		//_okno.draw(box);
		if (win)
			win_message();
		_okno.draw(text);
		_okno.display();
	}
}

std::vector<std::string> Game::image_files_list()
{
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	TCHAR tmp[300];
	const std::vector<std::string> file_extension{ "jpg", "png", "bmp" };

	GetModuleFileName(NULL, tmp, 300);
	std::string dir(tmp);
	dir.replace(dir.find_last_of('\\'), dir.size(), "\\*");

	//std::cout << dir;

	hFind = FindFirstFile(dir.c_str(), &ffd);

	std::string name;
	std::vector<std::string> File_Tab;

	do
	{
		if (~ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			name = ffd.cFileName;
			std::string t = name.substr(name.find_last_of('.') + 1);
			std::transform(t.begin(), t.end(), t.begin(), tolower);
			for (auto &str : file_extension)
				if (str == t)
					File_Tab.push_back(name);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);
	return File_Tab;
}

int Game::main_menu(sf::RenderWindow &window)
{
	//sf::RenderWindow window(sf::VideoMode(400, 400), "Gra - Menu glowne", sf::Style::Titlebar);

	window.setTitle("Gra - Menu glowne");
	window.setSize(sf::Vector2u(400, 400));
	sf::View view(sf::FloatRect(0.f, 0.f, 400.f, 400.f));
	window.setView(view);

	Options opt;

	Controls_Box box1(10, 10, 250, 60, "Typ kafelkow");
	//Controls_Box box2(10, 85, 250, 100, "Rozmiar kafelkow");
	//Controls_Box box3(10, 200, 250, 100, "Liczba kafelkow");
	//std::vector<Controls_Box*> box_tab;
	//box_tab.push_back(&box1);
	//box_tab.push_back(&box2);
	//box_tab.push_back(&box3);
	int pos_tab = 0;

	Button btn_num(24, 10, 90, 20, "Zwykle"), btn_img(24, 34, 90, 20, "Obrazkowe");
	box1.add(btn_num);
	box1.add(btn_img);
		//box1.active();
	int men_num = 1;

	int pos = 0;
	btn_num.click();

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Up)
				{
					btn_img.click();
					btn_num.click();
					pos = (pos) ? 0 : 1;
				}
				else if (event.key.code == sf::Keyboard::Down)
				{
					btn_img.click();
					btn_num.click();
					pos = (pos) ? 0 : 1;
				}
				else if (event.key.code == sf::Keyboard::Left)
				{

				}
				else if (event.key.code == sf::Keyboard::Right)
				{

				}
				else if (event.key.code == sf::Keyboard::Return)
				{
					//(pos) ? btn_img.click() : btn_num.click();
					//box_tab[pos_tab]->active();
					//if (++pos_tab == 3)
					//	return 0;
					//box_tab[pos_tab]->active();
					return pos;

				}
			}
		}

		window.clear(sf::Color::White);
		//window.draw(btn_num);
		//window.draw(btn_img);

		window.draw(box1);
		//window.draw(box2);
		//window.draw(box3);
		window.display();
	}
}

std::string Game::file_menu(sf::RenderWindow &window)
{	
	std::vector<std::string> file_list = image_files_list();
	std::vector<Button> buttons_list;
	sf::Text nazwa_pliku;
	int file_num = file_list.size();
	int pos = 0;

	//sf::RenderWindow okno_plikow(sf::VideoMode(400, file_num * 32 + 3 - 5), "Lista plikow", sf::Style::Titlebar);

	window.setSize(sf::Vector2u(400, (file_num + 1) * 32 + 3 - 5));
	sf::View view(sf::FloatRect(0.f, 0.f, 400.f, float((file_num + 1) * 32 + 3 - 5)));
	window.setView(view);
	window.setTitle("Lista plikow");

	int i;
	for (i = 0; i < file_num; i++)
		buttons_list.push_back(Button(3, 3 + i * 32, 394, 24, file_list[i]));
	buttons_list.push_back(Button(3, 3 + i * 32, 50, 24, "Cofnij"));
	buttons_list[0].click();

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Up)
				{
					buttons_list[pos].click();
					if (--pos < 0)
						pos = file_num;
					buttons_list[pos].click();
				}
				else if (event.key.code == sf::Keyboard::Down)
				{
					buttons_list[pos].click();
					if (++pos == file_num + 1)
						pos = 0;
					buttons_list[pos].click();
				}
				else if (event.key.code == sf::Keyboard::Return)
				{
					if (pos == file_num)
						return "";
					else
						return file_list[pos];
				}
			}
		}

		window.clear(sf::Color::White);
		for (auto &p : buttons_list)
			window.draw(p);
		window.display();
	}
}


// KLASA LABEL
sf::Font Label::_font;

Label::Label(int x, int y, const std::string &String, sf::Color Text_Color, std::size_t Text_Size, sf::Color Background_Color, std::size_t Outline_Thickness, sf::Color Outline_Color)
{
	_font.loadFromFile("arial.ttf");

	_txt.setFont(_font);
	_txt.setCharacterSize(Text_Size);
	_txt.setString(String);
	_txt.setPosition(float(x), float(y));
	_txt.setColor(Text_Color);

	_box.setFillColor(Background_Color);
	_box.setOutlineThickness(Outline_Thickness);
	_box.setOutlineColor(Outline_Color);
	_box.setPosition(float(x), float(y + text_top(_txt)));
	_box.setSize(sf::Vector2f(float(text_width(_txt) + text_left(_txt)), float(text_height(_txt))));
}


// KLASA BUTTON
sf::Font Button::_font;

Button::Button(int x, int y, int w, int h, const std::string &str) : _wsp(x, y, w, h), clicked(false)
{
	_font.loadFromFile("arial.ttf");

	_txt.setFont(_font);
	_txt.setCharacterSize(16);
	_txt.setString(str);
	_txt.setPosition(float(_wsp.left + 2), float(_wsp.top));
	_txt.setColor(sf::Color::Black);
	
	_shp_normal.setPosition(float(_wsp.left), float(_wsp.top));
	_shp_normal.setSize(sf::Vector2f(float(_wsp.width), float(_wsp.height)));
	_shp_normal.setFillColor(sf::Color::White);
	_shp_normal.setOutlineThickness(1);
	_shp_normal.setOutlineColor(sf::Color::Transparent);

	_shp_clicked.setPosition(float(_wsp.left), float(_wsp.top));
	_shp_clicked.setSize(sf::Vector2f(float(_wsp.width), float(_wsp.height)));
	_shp_clicked.setFillColor(sf::Color(211, 229, 252));
	_shp_clicked.setOutlineThickness(1);
	_shp_clicked.setOutlineColor(sf::Color(125, 162, 206));
}

Button::Button(int x, int y, const std::string &str) : _wsp(x, y, 0, 0), clicked(false)
{
	_font.loadFromFile("arial.ttf");

	_txt.setFont(_font);
	_txt.setCharacterSize(16);
	_txt.setString(str);
	_txt.setPosition(float(_wsp.left + 2), float(_wsp.top));
	_txt.setColor(sf::Color::Black);

	_wsp.width = text_width(_txt) + 6;
	_wsp.height = text_height(_txt);

	_shp_normal.setPosition(float(_wsp.left), float(_wsp.top));
	_shp_normal.setSize(sf::Vector2f(float(_wsp.width), float(_wsp.height)));
	_shp_normal.setFillColor(sf::Color::White);
	_shp_normal.setOutlineThickness(1);
	_shp_normal.setOutlineColor(sf::Color::Transparent);

	_shp_clicked = _shp_normal;
}


// KLASA CONTROLS_BOX
sf::Font Controls_Box::_font;

Controls_Box::Controls_Box(int x, int y, int w, int h, const std::string &str) : _wsp(x, y, w, h), _active(false), _txt(x+20, y-8, str, sf::Color::Black, 10, sf::Color::White, 0, sf::Color::Transparent)
{
	_font.loadFromFile("arial.ttf");

	_box.setPosition(sf::Vector2f(float(x), float(y)));
	_box.setSize(sf::Vector2f(float(w), float(h)));
	_box.setFillColor(sf::Color::Transparent);
	_box.setOutlineThickness(2);
	_box.setOutlineColor(sf::Color::Black);

	_box_active = _box;
	_box_active.setOutlineColor(sf::Color::Red);
}

void Controls_Box::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	if (_active)
		target.draw(_box_active);
	else
		target.draw(_box);
	target.draw(_txt);

	for (auto &e : _elem_tab)
		target.draw(*e, states);
}
