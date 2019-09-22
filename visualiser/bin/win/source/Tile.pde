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

  int[] getConnectionsFromId(int id) {
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

  void drawAt(int x, int y, int size) {
    noFill();  
    stroke(0);
    strokeWeight(2);
    rect(x, y, size, size);

    if (empty) {
      fill(0);
      rect(x, y, size, size);
      return;
    }

    int leftPoint = (int) (size * 0.33);
    int rightPoint = (int) (size * 0.66);

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



  void drawLines(PVector[] in, PVector[] out) {
    strokeWeight(1);

    drawLine(in[N], out[connections[N]], N, connections[N]);
    drawLine(in[E], out[connections[E]], E, connections[E]);
    drawLine(in[S], out[connections[S]], S, connections[S]);
    drawLine(in[W], out[connections[W]], W, connections[W]);
  }

  void drawLine(PVector from, PVector to, int fromDir, int toDir) {
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
      lowerBound = PI * 1.5;
    }

    // Is it a loop?
    if (fromDir == toDir) {
      PVector midPoint = PVector.add(from, PVector.sub(to, from).mult(0.5));
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
      lowerBound -= PI * 1.5;
      arc(corner.x, corner.y, ecc*2, ecc*2, lowerBound , lowerBound + PI/2);
    }
  }
}