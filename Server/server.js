import Koa from 'koa';
import Router from 'koa-router';
import bodyParser from '@koa/bodyparser';
import { Game } from './game.js';

const app = new Koa();

// logger
app.use(async (ctx, next) => {
    await next();
    const rt = ctx.response.get('X-Response-Time');
    console.log(`${ctx.method} ${ctx.url} - ${rt}`);
});

// x-response-time
app.use(async (ctx, next) => {
    const start = Date.now();
    await next();
    const ms = Date.now() - start;
    ctx.set('X-Response-Time', `${ms}ms`);
});

// body parsing
app.use(bodyParser());

// game state
const game = new Game();
let stateStringified = JSON.stringify(game.getState());

const router = new Router();
router.get('/state', (ctx, next) => {
    if (game.hasStateChanged()) stateStringified = JSON.stringify(game.getState());
    ctx.body = stateStringified;
    ctx.set('Content-Type', 'application/json');
});

router.post('/start', (ctx, next) => {
    game.start();
});

router.post('/player', (ctx, next) => {
    const id = game.addPlayer()
    ctx.body = { id };
});

router.put('/player/:id/move', (ctx, next) => {
    const playerId = ctx.params.id;
    const body = ctx.request.body;
    game.move(playerId, body.vector);
    ctx.body = game.getState().players[playerId];
});

router.put('/player/:id/take', (ctx, next) => {
    const playerId = ctx.params.id;
    const body = ctx.request.body;
    game.take(playerId, body.id);
    ctx.body = game.getState().players[playerId];
});

router.put('/player/:id/drop', (ctx, next) => {
    const playerId = ctx.params.id;
    const body = ctx.request.body;
    game.drop(playerId, body.type);
    ctx.body = game.getState().players[playerId];
});

router.put('/player/:id/attack', (ctx, next) => {
    const playerId = ctx.params.id;
    const body = ctx.request.body;
    game.attack(playerId, body.id);
    ctx.body = game.getState().players[body.id];
});

app.use(router.routes())

app.listen(3000, () => {
    console.log('Server running on https://localhost:3000')
});
