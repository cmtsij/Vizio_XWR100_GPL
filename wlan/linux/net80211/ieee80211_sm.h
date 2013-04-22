/*
 * Copyright (c) 2009, Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */

/*
* if_ath_sm.h - State machine definitions for if_ath layer
*
*/

#ifndef IEEE80211_SM_H
#define IEEE80211_SM_H

/**
 * SM_STATE - Declaration of a state machine function
 * @machine: State machine name
 * @state: State machine state
 *
 * This macro is used to declare a state machine function. It is used in place
 * of a C function definition to declare functions to be run when the state is
 * entered by calling SM_ENTER or SM_ENTER_GLOBAL.
 */

#define SM_STATE(machine, state) \
static void sm_ ## machine ## _ ## state ## _Enter(STATE_MACHINE_DATA *sm)


/**
 * SM_ENTRY - State machine function entry point
 * @machine: State machine name
 * @state: State machine state
 *
 * This macro is used inside each state machine function declared with
 * SM_STATE. SM_ENTRY should be in the beginning of the function body, but
 * after declaration of possible local variables. This macro prints debug
 * information about state transition and update the state machine state.
 */
#define SM_ENTRY(machine, _state, data) \
sm->data ## _ ## state = machine ## _ ## _state;


/**
 * SM_ENTER - Enter a new state machine state
 * @machine: State machine name
 * @state: State machine state
 *
 * This macro expands to a function call to a state machine function defined
 * with SM_STATE macro. SM_ENTER is used in a state machine step function to
 * move the state machine to a new state.
 */
#define SM_ENTER(machine, state) \
sm_ ## machine ## _ ## state ## _Enter(sm)		


/**
 * SM_STEP - Declaration of a state machine step function
 * @machine: State machine name
 *
 * This macro is used to declare a state machine step function. It is used in
 * place of a C function definition to declare a function that is used to move
 * state machine to a new state based on state variables. This function uses
 * SM_ENTER and SM_ENTER_GLOBAL macros to enter new state.
 */
#define SM_STEP(machine) \
static void sm_ ## machine ## _Step(STATE_MACHINE_DATA *sm)		

/**
 * SM_STEP_RUN - Call the state machine step function
 * @machine: State machine name
 *
 * This macro expands to a function call to a state machine step function
 * defined with SM_STEP macro.
 */
#define SM_STEP_RUN(machine) sm_ ## machine ## _Step(sm)

#endif /* IEEE80211_SM_H */
