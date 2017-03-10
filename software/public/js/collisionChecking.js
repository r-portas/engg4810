/* exported collision */

/**
 * Checks for a collision between a set of lines
 * @author Roy Portas <royportas@gmail.com>
 */

const collision = {
  /**
   * Gets the collision object
   * with all fields calculated
   */
  getObject(x1, y1, x2, y2) {
    return {
      maxX: Math.max(x1, x2),
      minX: Math.min(x1, x2),
      maxY: Math.max(y1, y2),
      minY: Math.min(y1, y2),
      point1: { x: x1, y: y1 },
      point2: { x: x2, y: y2 },
    };
  },

  getLineFormula(obj) {
    const m = (obj.point1.y - obj.point2.y) /
      (obj.point1.x - obj.point2.x);

    // y = mx + c
    // c = y - mx
    const c = obj.point1.y - (m * obj.point1.x);

    return { m, c };
  },

  /**
   * The first step of checking for a collision
   *
   * If it detects a collision here, then
   * the system should verify by using the
   * line equation
   */
  checkBoundingBoxCollision(box1, box2) {
    if (box1.minX < box2.maxX &&
        box1.maxX > box2.minX &&
        box1.minY < box2.maxY &&
        box1.maxY > box2.minY) {
      return true;
    }

    return false;
  },
};
