#N specifies the number of random numbers to be drawn
function serial_pi{T<:Integer}(N::T)
    inside::Float64 = 0
    outside::Float64 = 0
    radius::Float64 = 1
    for i in 1:N
        r = rand(2)
        if !(sum(r.^2) > radius)
            inside += 1
        end
    end
    inside/N*4
end

function parallel_pi{T<:Integer}(N::T)
    inside::Float64 = 0
    outside::Float64 = 0
    radius::Float64 = 1
    randArray = zeros(N,2)
    inside =  @parallel (+) for i in 1:N
        r = rand(2)
        if sum(r.^2) < radius
            1
        else
            0
        end
    end
    inside/N*4
end


function parallel_pi{T<:Integer}(N::T)
    inside::Float64 = 0
    outside::Float64 = 0
    radius::Float64 = 1
    randArray = zeros(N,2)
    inside =  @parallel (+) for i in 1:N
        r = rand(2)
        if sum(r.^2) < radius
            1
        else
            0
        end
    end
    inside/N*4
end

@everywhere function comp_pi(r1,r2)
    radius = 1
    if r1^2 + r2^2 < radius
        return 1
    else
        return 0
    end
end

function pmap_pi{T<:Integer}(N::T)
    res = pmap(comp_pi,rand(N),rand(N))
    sum(res)/N*4
end

function pmap_pi_fast{T<:Integer}(N::T, nchunks::T)
end

@everywhere function computeChunk!(R,startindex, chunksize)
    for i in startindex:(startindex+chunksize)
        r = rand(2)
        if sum(r.^2) < 1
            R[i] = 1
        end
    end
    R
end

function processLength(N)
    nprocs = length(procs())
    (chunksize,r) = divrem(N,nprocs)
    if r > 0
        chunks = [repmat([chunksize],nprocs-1);r]
    else
        chunks = repmat([chunksize],nprocs)
    end
    return chunks
end

function shared_pi{T<:Integer}(N::T)
    R = SharedArray(Float64, (N,1))#, init= R -> R[Base.localindexes(R)] = rand(N,2))
    chunks = processLength(N)
    ps = procs()
    @sync begin
        for i in 1:length(ps)
            @async remotecall_wait(ps[i],computeChunk!,R,(i-1)*chunks[1]+1,chunks[i]-1)
        end
    end
    sum(R)/N*4
end
#println(pmap_pi(1000))
#println(pmap_pi(1000000))
#pmap_pi(1000000)
#pmap_pi(1000000000)

