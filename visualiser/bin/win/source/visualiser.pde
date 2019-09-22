public static final int N = 0;
public static final int E = 1;
public static final int S = 2;
public static final int W = 3;

Tile[][] tiles;

void setup() {
  size(640, 640);
  tiles = new Tile[8][8];
  loadGrid();
}

void loadGrid() {
  String path = "../../../output.txt";
  String[] lines = loadStrings(path);

  int row = 0;

  for (String line : lines) {
    String[] tokens = line.split(" ");
    int column = 0;   
    for (String token : tokens) {
      if (token.equals("-"))
        tiles[row][column++] = new Tile();
      else
        tiles[row][column++] = new Tile(Integer.parseInt(token));
    }
    row++;
  }
  // TODO: DO it!
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