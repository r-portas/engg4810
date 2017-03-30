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

  /**
   * Gets the orientation of the line
   */
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

  /**
   * Checks if two line segments intersect
   */
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
   * Converts an array of data to line segments
   */
  convertArrayToSegments(array) {
    if (array.length < 2) {
      return [];
    }

    const segments = [];

    for (let i = 1; i < array.length; i += 1) {
      const item = array[i];
      const lastItem = array[i - 1];

      if (!isNaN(item.x) && !isNaN(item.y) && !isNaN(lastItem.x) && !isNaN(lastItem.y)) {
        segments.push(this.getLineSegment(item.x, item.y, lastItem.x, lastItem.y));
      }
    }

    return segments;
  },

  /**
   * Checks if the data has collisions against the mask
   */
  checkCollision(data, topMask, bottomMask) {
    const topMaskSegments = this.convertArrayToSegments(topMask);
    const bottomMaskSegments = this.convertArrayToSegments(bottomMask);
    const dataSegments = this.convertArrayToSegments(data);

    // Store a list of collisions
    const collisions = [];

    for (let i = 0; i < dataSegments.length; i += 1) {
      const dataItem = dataSegments[i];

      for (let j = 0; j < topMaskSegments.length; j += 1) {
        const topMaskItem = topMaskSegments[j];
        if (this.checkIntersect(dataItem, topMaskItem)) {
          collisions.push(['Top Mask', dataItem, topMaskItem]);
          console.log('Found collision between top mask and line segment');
        }
      }

      for (let j = 0; j < bottomMaskSegments.length; j += 1) {
        const bottomMaskItem = bottomMaskSegments[j];
        if (this.checkIntersect(dataItem, bottomMaskItem)) {
          collisions.push(['Bottom Mask', dataItem, bottomMaskItem]);
          console.log('Found collision between bottom mask and line segment');
        }
      }
    }

    for (let i = 0; i < bottomMaskSegments.length; i += 1) {
      const bottomMaskItem = bottomMaskSegments[i];
      for (let j = 0; j < topMaskSegments.length; j += 1) {
        const topMaskItem = topMaskSegments[j];

        if (this.checkIntersect(bottomMaskItem, topMaskItem)) {
          console.log('Detected invalid mask configuration');
          collisions.push(['Invalid Mask Configuration', topMaskItem, bottomMaskItem]);
        }
      }
    }

    return collisions;
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
