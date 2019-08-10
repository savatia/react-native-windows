// For a detailed explanation regarding each configuration property, visit:
// https://jestjs.io/docs/en/configuration.html

const {defaults} = require('jest-config');

module.exports = {

  moduleFileExtensions: [...defaults.moduleFileExtensions, 'ts'],

  projects: ['e2e'],
  testEnvironment: "node",
};
