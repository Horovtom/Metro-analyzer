public static final int N = 0; //<>// //<>// //<>//
public static final int E = 1;
public static final int S = 2;
public static final int W = 3;
public static final int UNSET = -1;

Tile[][] tiles;

int startX, startY, startDirection = UNSET;

void setup() {
  size(640, 640);
  tiles = new Tile[8][8];
  loadGrid();
  loadHighlightedPath();
}

void loadHighlightedPath() {
  int x = startX, y = startY;
  int direction = startDirection;
  if (direction == UNSET) return;

  while (true) {
    Tile t = tiles[x][y];
    if (t == null || t.empty) return;
    t.highlight[direction] = true; //<>//
    direction = t.connections[direction];
    if (direction == N)
      y--;
    else if (direction == S) 
      y++;
    else if (direction == W) 
      x--;
    else if (direction == E)
      x++;
    else 
    break;

    direction = flip(direction);

    // Bound detection
    if (x < 0 || x >=8 || y < 0 || y >= 8) break;

    // Cycle detection
    if (x == startX && y == startY && direction == startDirection) return;
  }
}

void loadGrid() {
  String path = "../../../output.txt";
  //String path = "/home/lactosis/Documents/Programming/C++/Metro/output.txt";
  String[] lines = loadStrings(path);

  int row = -1;


  for (String line : lines) {
    if (row == -1) {
      // Initialization line
      if ( line.length() > 0) {
        String[] split = line.split(" ");
        if (split.length > 0) {
          if (split.length != 3) {
            println("INVALID FORMAT!");
            exit();
          }

          startX = Integer.parseInt(split[0]);
          startY = Integer.parseInt(split[1]);
          startDirection = Integer.parseInt(split[2]);
        }
      }
    } else {
      String[] tokens = line.split(" ");
      int column = 0;   
      for (String token : tokens) {
        if (token.equals("-"))
          tiles[row][column++] = new Tile();
        else
          tiles[row][column++] = new Tile(Integer.parseInt(token));
      }
    }
    row++;
  }
}

void draw() {
  background(255);
  int squareSize = width / 8;

  for (int i = 0; i < 8; ++i) 
    for (int j = 0; j < 8; ++j) {
      if (tiles[i][j] != null)
        tiles[i][j].drawAt(i*squareSize, j*squareSize, squareSize);
      else
      {
        fill(0, 0, 255);
        rect(i*squareSize, j*squareSize, squareSize, squareSize);
        noFill();
      }
    }
}

int flip(int dir) {
  switch (dir) {
  case N:
    return S;
  case S:
    return N;
  case E:
    return W;
  case W:
    return E;
  } 
  return -1;
}

void keyPressed() {
  if (key == ' ') {
    loadGrid();
  }
}

void mousePressed() {
  int squareSize = width / 8;
  int x = mouseX / squareSize;
  int y = mouseY / squareSize;
  tiles[x][y] = null;
}
