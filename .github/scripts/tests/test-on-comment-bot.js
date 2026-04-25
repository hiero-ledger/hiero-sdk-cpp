const { parseComment } = require('../bot-on-comment');

describe('parseComment', () => {
  test('exact assign', () => {
    expect(parseComment('/assign')).toEqual({ commands: ['assign'] });
  });

  test('near miss assign', () => {
    expect(parseComment('/assign hi')).toEqual({ nearMiss: 'assign' });
  });

  test('exact unassign', () => {
    expect(parseComment('/unassign')).toEqual({ commands: ['unassign'] });
  });

  test('near miss finalize', () => {
    expect(parseComment('/finalize now')).toEqual({ nearMiss: 'finalize' });
  });

  test('unknown', () => {
    expect(parseComment('hello')).toEqual({ commands: [] });
  });

  test('empty', () => {
    expect(parseComment('')).toEqual({ commands: [] });
  });
});