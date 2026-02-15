// SPDX-License-Identifier: Apache-2.0
//
// bot-on-comment.js
//
// Handles issue comment events: reads the comment body, parses commands, and dispatches
// to the appropriate handler. Implemented commands: /assign (more may be added later).
//
// /assign: see commands/assign.js (skill levels, assignment limits, required labels).

const { createLogger, buildBotContext } = require('./helpers');
const { handleAssign } = require('./commands/assign');

const logger = createLogger('on-comment');

// =============================================================================
// COMMAND PARSING
// =============================================================================

/**
 * Parses the comment body and returns the list of commands to run.
 * Commands are recognized by exact match (with optional surrounding whitespace).
 *
 * @param {string} body - The comment body.
 * @returns {{ commands: string[] }} - List of command names (e.g. ['assign'] or []).
 */
function parseComment(body) {
  if (typeof body !== 'string') {
    return { commands: [] };
  }
  if (/^\s*\/assign\s*$/i.test(body)) {
    logger.log('parseComment: detected /assign');
    return { commands: ['assign'] };
  }
  logger.log('parseComment: no known command', { body: body.substring(0, 80) });
  return { commands: [] };
}

// =============================================================================
// ENTRY POINT
// =============================================================================

/**
 * Entry point: read comment, parse commands, dispatch to handlers.
 * Validates that the event is a comment from a human; then runs each detected command.
 */
module.exports = async ({ github, context }) => {
  try {
    const botContext = buildBotContext({ github, context });

    if (!botContext.comment?.user?.login) {
      logger.log('Exit: missing comment user login');
      return;
    }

    if (botContext.comment.user.type === 'Bot') {
      logger.log('Exit: comment authored by bot');
      return;
    }

    const parsed = parseComment(botContext.comment.body);
    if (parsed.commands.length === 0) {
      logger.log('Exit: no known command');
      return;
    }

    for (const command of parsed.commands) {
      if (command === 'assign') {
        await handleAssign(botContext);
      } else {
        logger.log('Unknown command:', command);
      }
    }
  } catch (error) {
    logger.error('Error:', {
      message: error.message,
      status: error.status,
      number: context.payload.issue?.number,
      commenter: context.payload.comment?.user?.login,
    });
    throw error;
  }
};
