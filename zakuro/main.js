const puppeteer = require('puppeteer');
const process = require('process')
const fs = require('fs')
const path = require('path')

const vis = process.env.VIS || 0;

if(process.argv.length < 5) {
  console.log('node main.js {FA|FD|FR} [model dir] [trace dir] [range (optional)]')
  console.log('range example: 114-514')
}

let [type, modelDir, traceDir, range] = process.argv.slice(2).concat([null]);
let isinRange = (i => {
  if(range === null) return true;
  else {
    let rs = range.split('-').map(e => parseInt(e));
    if(rs.length === 1) rs.push(rs[0]);
    return rs[0] <= i && i <= rs[1];
  }
});
let files = [];

fs.readdirSync(modelDir).filter(f => f.startsWith(type) && f.endsWith('.mdl')).forEach(file => {
  let num = parseInt(file.match(/\d+/)[0], 10);
  files[num] = files[num] || {};
  if(file.indexOf("src") !== -1) files[num].modelsrc = path.join(modelDir, file);
  if(file.indexOf("tgt") !== -1) files[num].modeltgt = path.join(modelDir, file);
});
fs.readdirSync(traceDir).filter(f => f.startsWith(type) && f.endsWith('.nbt')).forEach(file => {
  let num = parseInt(file.match(/\d+/)[0], 10);
  files[num].trace = path.join(traceDir, file);
});

console.log("Case,Time,Energy,ClockTime");

(async () => {
  const browser = await puppeteer.launch({headless: vis == 0});
  const page = await browser.newPage();
  await page.goto(
    vis > 0 ? 'https://icfpcontest2018.github.io/full/exec-trace.html' :
          'https://icfpcontest2018.github.io/full/exec-trace-novis.html'
  );
  await page.evaluate((v) => {
    let boost = document.querySelector('option');
    boost.value = v;
    boost.innerHTML = v;
    boost.selected = true;
  }, vis);
  let idx = 0;
  for(let file of files) {
    if(!file) continue;
    idx++;
    if(!isinRange(idx)) continue;

    await page.evaluate((s, t) => {
      document.querySelector('#srcModelEmpty').checked = s;
      document.querySelector('#tgtModelEmpty').checked = t;
    }, !file.modelsrc, !file.modeltgt);

    if(file.modelsrc)
      await (await page.$('#srcModelFileIn')).uploadFile(file.modelsrc);
    if(file.modeltgt)
      await (await page.$('#tgtModelFileIn')).uploadFile(file.modeltgt);

    await (await page.$('#traceFileIn')).uploadFile(file.trace);
    await (await page.$('#execTrace')).click();
    await page.waitForFunction('!document.getElementById("execTrace").disabled', { timeout: 0 });
    if(vis > 0) page.waitFor(3000);

    let result = await (await (await page.$("#stdout")).getProperty('textContent')).jsonValue();
    if(result.startsWith('Success')) {
      let lines = result.split('\n');
      console.log([idx].concat(lines.slice(1,5).map(s => parseInt(s.match(/\d+/)[0], 10))).join(','));
    } else {
      console.error("Error!");
      console.error(result);
      await browser.close();
      process.exit(1);
    }
  }

  await browser.close();
})();