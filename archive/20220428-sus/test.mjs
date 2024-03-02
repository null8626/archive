import { sep } from "node:path";
import { execSync } from "node:child_process";
import { existsSync } from "node:fs";
import { strict } from "node:assert";
import test from "fast-fail";

const exe = process.platform.startsWith("win") ? ".exe" : "";

test("Existence of the sus executable", () => {
  strict.equal(existsSync(`.${sep}sus${exe}`), true);
});

test("Compiling helloworld.sus", () => {
  execSync(`.${sep}sus${exe} .${sep}helloworld.sus`);
  strict.equal(existsSync(`.${sep}helloworld${exe}`), true);
});

test(`Running helloworld${exe}`, () => {
  strict.equal(execSync(`.${sep}helloworld${exe}`).toString().trim(), "Hello, world!");
});