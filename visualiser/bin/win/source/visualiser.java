import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class visualiser extends PApplet {

public static final int N = 0;
public static final int E = 1;
public static final int S = 2;
public static final int W = 3;

Tile[][] tiles;

public void setup() {
  
  tiles = new Tile[8][8];
  loadGrid();
}

public void loadGrid() {
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

public void draw() {
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

public int flip(int dir) {
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

public void keyPressed() {
  if (key == ' ') {
    loadGrid();
  }
}

public void mousePressed() {
  int squareSize = width / 8;
  int x = mouseX / squareSize;
  int y = mouseY / squareSize;
  tiles[x][y] = null;
}
class Tile {
  int[] connections;
  boolean empty;

  Tile() {
    empty = true;
  }

  Tile(int id) {
    empty = false;
    connections = getConnectionsFromId(id);
  }

  public int[] getConnectionsFromId(int id) {
    int[] res = new int[4];
    res[N] = id >> 3;
    id -= res[N] << 3;
    res[E] = id >> 1;
    id -= res[E] << 1;

    int start, p;
    if (id == 0) {
      start = 0;
      p = 1;
    } else {
      start = 3;
      p = -1;
    }

    int toFind = S;

    for (int i = start; i >= 0 && i < 4; i += p) {
      if (res[N] != i && res[E] != i) {
        res[toFind] = i;
        toFind = W;
      }
    }

    return res;
  }

  public void drawAt(int x, int y, int size) {
    noFill();  
    stroke(0);
    strokeWeight(2);
    rect(x, y, size, size);

    if (empty) {
      fill(0);
      rect(x, y, size, size);
      return;
    }

    int leftPoint = (int) (size * 0.33f);
    int rightPoint = (int) (size * 0.66f);

    PVector[] in = new PVector[] {
      new PVector(x + leftPoint, y), 
      new PVector(x + size, y + leftPoint), 
      new PVector(x + rightPoint, y+size), 
      new PVector(x, y +rightPoint)
    };

    PVector[] out = new PVector[] {
      new PVector(x + rightPoint, y), 
      new PVector(x + size, y+ rightPoint), 
      new PVector(x + leftPoint, y + size), 
      new PVector(x, y + leftPoint)
    };
    // Lines
    drawLines(in, out);
  }



  public void drawLines(PVector[] in, PVector[] out) {
    strokeWeight(1);

    drawLine(in[N], out[connections[N]], N, connections[N]);
    drawLine(in[E], out[connections[E]], E, connections[E]);
    drawLine(in[S], out[connections[S]], S, connections[S]);
    drawLine(in[W], out[connections[W]], W, connections[W]);
  }

  public void drawLine(PVector from, PVector to, int fromDir, int toDir) {
    stroke(255, 0, 0);
    strokeWeight(1);
    // Is it a straight line?
    if (flip(fromDir) == toDir) {   
      line(from.x, from.y, to.x, to.y);
      return;
    }

    // Find arc boundaries
    float lowerBound = 0;

    if (fromDir == N) {
      lowerBound = 0;
    } else if (fromDir == E) {
      lowerBound = PI/2;
    } else if (fromDir == S) {
      lowerBound = PI;
    } else {
      lowerBound = PI * 1.5f;
    }

    // Is it a loop?
    if (fromDir == toDir) {
      PVector midPoint = PVector.add(from, PVector.sub(to, from).mult(0.5f));
      int ecc = (int) PVector.dist(midPoint, from);

      arc(midPoint.x, midPoint.y, ecc*2, ecc*2, lowerBound, lowerBound + PI);
      return;
    }
    
    // So it is a curve then...
    PVector corner;
    if (fromDir % 2 == 0) {
      corner = new PVector(to.x, from.y);
    } else {
      corner = new PVector(from.x, to.y);
    }
    int ecc = (int) PVector.dist(corner, from);
    
    if ((fromDir - toDir + 4) % 4 == 1) {
      // We are turning right, do the small arc
      
      arc(corner.x, corner.y, ecc*2, ecc*2, lowerBound, lowerBound + PI/2);
    } else {
      // We are turning left, do the large arc
      lowerBound -= PI * 1.5f;
      arc(corner.x, corner.y, ecc*2, ecc*2, lowerBound , lowerBound + PI/2);
    }
  }
}
  public void settings() {  size(640, 640); }
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "visualiser" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
