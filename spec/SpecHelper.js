/* eslint no-console: off */

describe('SpecHelper', () => {
  let oldError;

  beforeAll(() => {
    oldError = console.error;

    console.error = (message) => {
      if (!message.includes('Warning')) {
        throw new Error(message);
      }
    };
  });

  afterAll(() => {
    console.error = oldError;
    return console.error;
  });
});
