/* eslint-disable */

/**
 * Tests the collision checking code
 * @author Roy Portas
 */
const assert = chai.assert;

describe('Collision', () => {
  describe('getSegment', () => {
    it('Should correctly create a collision object with given parameters', () => {
      let obj = collision.getLineSegment(1, 2, 3, 4);

      assert.equal(obj.maxX, 3, 'Max x should be correct');
      assert.equal(obj.minX, 1, 'Min x should be correct');
      assert.equal(obj.maxY, 4, 'Max y should be correct');
      assert.equal(obj.minY, 2, 'Min y should be correct');

      assert.equal(obj.point1.x, 1, 'Point1.x should be correct');
      assert.equal(obj.point1.y, 2, 'Point1.y should be correct');
      assert.equal(obj.point2.x, 3, 'Point2.x should be correct');
      assert.equal(obj.point2.y, 4, 'Point2.y should be correct');
    });

  });

  describe('checkIntersect', () => {
    it('(1, 1) -> (10, 1) and (1, 2) -> (10, 2) does not intersect', () => {
      const seg1 = collision.getLineSegment(1, 1, 10, 1);
      const seg2 = collision.getLineSegment(1, 2, 10, 2);

      assert.isFalse(collision.checkIntersect(seg1, seg2));
      assert.isFalse(collision.checkIntersect(seg2, seg1));
    });

    it('(10, 0) -> (0, 10) and (0, 0) -> (10, 10) does intersect', () => {
      const seg1 = collision.getLineSegment(10, 0, 0, 10);
      const seg2 = collision.getLineSegment(0, 0, 10, 10);

      assert.isTrue(collision.checkIntersect(seg1, seg2));
      assert.isTrue(collision.checkIntersect(seg2, seg1));
    });

    it('(-5, -5) -> (0, 0) and (1, 1) -> (10, 10) does intersect', () => {
      const seg1 = collision.getLineSegment(-5, -5, 0, 0);
      const seg2 = collision.getLineSegment(1, 1, 10, 10);

      assert.isFalse(collision.checkIntersect(seg1, seg2));
      assert.isFalse(collision.checkIntersect(seg2, seg1));
    });
  });

  describe('getLineEquation', () => {
    it('Should correctly get a line for the points (1, 1) and (2, 2)', () => {
      let obj = collision.getLineSegment(1, 1, 2, 2);
      let equation = collision.getLineEquation(obj);

      assert.equal(equation.m, 1, 'The M component should be 1');
      assert.equal(equation.c, 0, 'The C component should be 0');
    });

    it('Should correctly get a line for the points (1, 3) and (5, -2)', () => {
      let obj = collision.getLineSegment(1, 3, 5, -2);
      let equation = collision.getLineEquation(obj);

      assert.equal(equation.m, -1.25, 'The M component should be -1.25');
      assert.equal(equation.c, 4.25, 'The C component should be 4.25');
    });

    it('Should correctly get a line for the points (5, 100) and (10, 1)', () => {
      let obj = collision.getLineSegment(5, 100, 10, 1);
      let equation = collision.getLineEquation(obj);

      assert.equal(equation.m, -19.8, 'The M component should be -19.8');
      assert.equal(equation.c, 199, 'The C component should be 199');
    });
  });

  describe('checkBoundingBoxCollision', () => {
    const seg1 = collision.getLineSegment(1, 1, 5, 5);
    const seg2 = collision.getLineSegment(2, 2, 5, 5);
    const seg3 = collision.getLineSegment(4, 2, 10, 10);
    const seg4 = collision.getLineSegment(1, 5, 5, 1);
    const seg5 = collision.getLineSegment(6, 6, 10, 10);

    it('Should correctly get a collision for seg1 and seg2', () => {
      assert.isTrue(collision.checkBoundingBoxCollision(seg1, seg2));
    });

    it('Should correctly get a collision for seg1 and seg3', () => {
      assert.isTrue(collision.checkBoundingBoxCollision(seg1, seg3));
    });

    it('Should correctly get a collision for seg1 and seg4', () => {
      assert.isTrue(collision.checkBoundingBoxCollision(seg1, seg4));
    });

    it('Should not get a collision for seg1 and seg5', () => {
      assert.isFalse(collision.checkBoundingBoxCollision(seg1, seg5));
    });

    it('Should  correctly get a collision for seg1 and seg1 (itself)', () => {
      assert.isTrue(collision.checkBoundingBoxCollision(seg1, seg1));
    });
  });

  describe('convertArrayToSegments', () => {
    it('Should return empty if array has less than 2 elements', () => {
      let data = [];

      assert.deepEqual(collision.convertArrayToSegments(data), []);

      data = [
        {x: 1, y: 1},
      ];

      assert.deepEqual(collision.convertArrayToSegments(data), []);
    });

    it('Should be able to convert a simple case with 2 points', () => {
      const data = [
        {x: 1, y: 1},
        {x: 2, y: 2},
      ];

      const expected = [
        collision.getLineSegment(2, 2, 1, 1),
      ];

      assert.deepEqual(collision.convertArrayToSegments(data), expected);

    });

    it('Should be able to convert 4 points', () => {
      const data = [
        {x: 1, y: 1},
        {x: 2, y: 2},
        {x: 3, y: 3},
        {x: 4, y: 4},
      ];

      const expected = [
        collision.getLineSegment(2, 2, 1, 1),
        collision.getLineSegment(3, 3, 2, 2),
        collision.getLineSegment(4, 4, 3, 3),
      ];

      assert.deepEqual(collision.convertArrayToSegments(data), expected);

    });
  });
});
