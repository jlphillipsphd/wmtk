## 1D TD-learning with HRRs proof of concept 
## Additional context component to signal
## 
## Copyright 2016 Joshua L. Phillips
source("hrr.r")
require(Matrix)

## Set the seed for repeatability
set.seed(0)

## This is essentially the length of the experiment
nstates <- 4

## Length of the HRRs
n <- 64

## Identity vector
hrr.i <- rep(0,n)
hrr.i[1] <- 1

## We encode each state as a unique HRR
states <- replicate(nstates,hrr(n,normalized=TRUE))

## Internal states need to be unique even
## though they signal items in the
## outside world. This is to ensure that
## concepts stored in working memory are
## tagged appropriately to a concept in
## the outside world, but are not
## represented by the -exact- same vector
## internally. This is needed in order
## to discriminate between -remembering-
## a concept and that concept being
## currently -present- in the environment.
internal <- as(sample(n),"pMatrix")
wm_metas <- apply(states,2,"%*%",internal)
wm_metas <- cbind(wm_metas,hrr.i)

## Now that we have all of the possible
## variables included, we will pre-compute
## all of the states for efficiency. Note
## that this is not needed, but makes things
## faster below...
inputs <- array(oconvolve(states,wm_metas),
                dim=c(n,nstates+1,nstates))

## We initialize the weight vector with small
## random values. Make this the last random
## initialization so that it can be commented
## out once the weights have been trained.
W <- rep(0,n)
W <- W + ((runif(n)*0.2)+0.01)
bias <- 0

## Optionally, we may want to start with an
## optimistic critic. This can easily be
## performed using SVD, but requires all
## states be enumerated. It might be possible
## to simplify this in some way by only
## selecting a subset of the states or
## an by using iterative approach.
## inputs.svd <- svd(t(matrix(inputs,nrow=n)))
## W <- as.vector(inputs.svd$v %*% diag(1/inputs.svd$d) %*% t(inputs.svd$u) %*%
##                    matrix(1,nstates*(nstates+1)))

## This vector will be updated on each step
## to reflect how the weights should change
## so that states[x] %*% W -> V[s].
## Basically, it only plays a significant
## role if lambda > 0.0 (below).
eligibility <- rep(0,n)

## Standard reward for non-goal states.
## Normally, this will be zero since there
## is no feedback at intermediate stages.
## However, other reward policies exist
## which will change the V[s] values but
## also speed convergence (like when
## setting to -1, for example).
default_reward <- 0.0

## Discounted future rewards
## V[s] = r[s] + gamma*V[s+1]
## delta[s] = (r[s] + gamma*V[s+1]) - V[s]
gamma <- 0.9

## For stability of learning, we don't
## update the weight vector using the
## exact delta[s] values. Instead, we
## use a fraction (lrate) of that update.
lrate <- 0.01

## Eligibility traces
## Under TD[lambda] rules, we share some
## of the information from our current
## delta[s] with states in the recent
## past since they also contributed to
## our current position. This can easily
## be set to 0.0 for standard TD-learning,
## or it can be set higher to speed
## learning (although values too close to
## 1.0 will make the equations unstable
## unless the lrate is also lowered
## to compensate).
lambda <- 0.3

## Epsilon-soft policy
epsilon <- 0.01

## We break learning up into episodes
count <- 0
for (episode in seq(1,100000)) {

    ## Set up list of states
    percepts <- sample(nstates)
    while (percepts[nstates] == 1)
        percepts <- sample(nstates)

    ## print("New episode")
    ## print(percepts)
    
    ## Clear weight eligibility vector
    eligibility <- rep(0,n)
    bias_e <- 0.0
    
    ## This is the agent's current thinking
    ## regarding the world.
    ## Initialize with a default expectation
    ## where it has no real idea of how to
    ## perform.
    current_wm <- nstates + 1

    ## Get ready for learning step
    previous_r <- 0.0
    previous_value <- 0.0
    
    ## Each episode will run until we reach the goal or
    ## for a maximum of 100 steps (we just start over
    ## again from the beginning if we haven't gotten
    ## to the goal in 100 steps).
    for (timestep in seq(1,nstates)) {

        ## Get current percept
        current <- percepts[timestep]    
        ## print(c("Before:",current,current_wm))

        ## Assume 0, but doesn't have to be in other problems...
        current_r <- default_reward
        ## Non-standard update - absorbed reward below
        if (timestep == nstates && current_wm == 1) {
            ## print("FOUND!")
            current_r <- 1.0
            count <- count + 1
        }

        ## Internal update of working memory is done -before-
        ## performing an action. Note this involves a juggling
        ## of the possible WM options, and loading up the one with
        ## the largest value. The TD error is then computed -after-
        ## since this reasoning process is considered "instantaneous".
        possible_wm <- sort(unique(c(current_wm,current,nstates+1)))
        ## On the absorb step, we don't update, it's just a silly thing to do...
        ## Note, everything will function fine with the update, so I have
        ## commented this step out, but the WMtk should -not- update WM
        ## on the absorb step.
        ## if (timestep == nstates)
        ##     possible_wm <- current_wm
        possible_values <- sapply(possible_wm,
                                  function(x) {
                                      as.vector(W%*%inputs[,x,current])+bias})
        
        wm_move <- which(possible_values==max(possible_values))[1]
        ## Epsilon-soft updates
        if (runif(1) < epsilon)
            wm_move <- sample(length(possible_values),1)
        ## Softmax WM updates
        ## wm_move <- which(runif(1)<cumsum(exp(possible_values/epsilon)/
        ##                                      sum(exp(possible_values/epsilon))))[1]

        ## Get information for TD-update
        current_wm <- possible_wm[wm_move]
        current_value <- possible_values[wm_move]

        ## print(sprintf("%d : %f",possible_wm,possible_values))
        ## print(c("After:",current,current_wm))

        ## Standard TD update
        td_error <- (previous_r + gamma*current_value) - previous_value
        ## print(c("Standard update:",sprintf("[(%f + %f*%f) - %f] = %f",
        ##                                    previous_r,gamma,current_value,
        ##                                    previous_value,td_error)))
        W <- W + lrate*eligibility*td_error
        bias <- bias + lrate*bias_e*td_error
        eligibility <- (lambda*eligibility) + inputs[,current_wm,current]
        bias_e <- 1.0
                
        ## Store previous information...
        previous <- current
        previous_r <- current_r
        previous_wm <- current_wm
        previous_value <- current_value        
    }
    
    eligibility <- (lambda*eligibility) + inputs[,previous_wm,previous]
    bias_e <- 1.0
    td_error <- previous_r - previous_value
    W <- W + lrate*eligibility*td_error
    bias <- bias + lrate*bias_e*td_error


    ## values <- matrix(as.vector(W%*%matrix(inputs,nrow=n)) + bias,
    ##                  ncol=ncol(wm_metas),nrow=ncol(states),byrow=TRUE)
    ## print(values)

    ## Print out status every 100 episodes
    freq <- 1000
    if (episode %% freq == 0) {
        ## Episode number and mean episode length for
        ## last 100 episodes
        print(sprintf("%d %f",episode,count/freq))
        count <- 0
        ## Get the value of all states
        values <- matrix(as.vector(W%*%matrix(inputs,nrow=n)) + bias,
                         ncol=ncol(wm_metas),nrow=ncol(states),byrow=TRUE)
        ## Plot the values
        matplot(values,xlab="s",ylab="V(s)",type='o',
                pch=19,lty=1:9,col=1:9,
                ylim=c(min(values,0),max(values,1.1)))
        legend("bottomright",
               legend=sprintf("WM%d",seq(1,nstates+1)),
               lty=1:9,col=1:9,pch=19)
    }
}

