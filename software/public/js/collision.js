/* exported collision */

/**
 * Checks for a collision between a set of lines
 * @author Roy Portas <royportas@gmail.com>
 */

const collision = {
  /**
   * Gets the line segment object
   * with all fields calculated
   */
  getLineSegment(x1, y1, x2, y2) {
    return {
      maxX: Math.max(x1, x2),
      minX: Math.min(x1, x2),
      maxY: Math.max(y1, y2),
      minY: Math.min(y1, y2),
      point1: { x: x1, y: y1 },
      point2: { x: x2, y: y2 },
    };
  },

  /**
   * Used the formula y = mx + c to generate the
   * equation for the line
   */
  getLineEquation(obj) {
    const m = (obj.point1.y - obj.point2.y) /
      (obj.point1.x - obj.point2.x);

    // y = mx + c
    // c = y - mx
    const c = obj.point1.y - (m * obj.point1.x);

    return { m, c };
  },

  onSegment(p, q, r) {
    if (q.x <= Math.max(p.x, r.x) && q.x >= Math.min(p.x, r.x) &&
      q.y <= Math.max(p.y, r.y) && q.y >= Math.min(p.y, r.y)) {
      return true;
    }

    return false;
  },

  getOrientation(p, q, r) {
    const val = ((q.y - p.y) * (r.x - q.x)) -
      ((q.x - p.x) * (r.y - q.y));

    if (val === 0) {
      return 0;
    }

    if (val > 0) {
      return 1;
    }

    // val < 0
    return 2;
  },

  checkIntersect(seg1, seg2) {
    const o1 = this.getOrientation(seg1.point1, seg1.point2, seg2.point1);
    const o2 = this.getOrientation(seg1.point1, seg1.point2, seg2.point2);
    const o3 = this.getOrientation(seg2.point1, seg2.point2, seg1.point1);
    const o4 = this.getOrientation(seg2.point1, seg2.point2, seg1.point2);

    // General case
    if (o1 !== o2 && o3 !== o4) {
      return true;
    }

    if (o1 === 0 && this.onSegment(seg1.point1, seg2.point1, seg1.point2)) {
      return true;
    }

    if (o2 === 0 && this.onSegment(seg1.point1, seg2.point2, seg1.point2)) {
      return true;
    }

    if (o3 === 0 && this.onSegment(seg2.point1, seg1.point1, seg2.point2)) {
      return true;
    }

    if (o4 === 0 && this.onSegment(seg2.point1, seg1.point2, seg2.point2)) {
      return true;
    }

    return false;
  },

  /**
   * The first step of checking for a collision
   *
   * If it detects a collision here, then
   * the system should verify by using the
   * line equation
   */
  checkBoundingBoxCollision(seg1, seg2) {
    if (seg1.minX < seg2.maxX &&
      seg1.maxX > seg2.minX &&
        seg1.minY < seg2.maxY &&
        seg1.maxY > seg2.minY) {
          return true;
        }

    return false;
  },
};
