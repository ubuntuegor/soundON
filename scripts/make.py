import sys

inputfile = sys.argv[1]
srt = list(map(lambda x: x.strip(), open(inputfile).readlines()))

i = 0

events = []
errors = []

def parseTime(time):
    s1 = time.split(":")
    h = int(s1[0])
    m = int(s1[1])
    s2 = s1[2].split(",")
    s = int(s2[0])
    ms = int(s2[1])

    return h * 60 * 60 + m * 60 + s + ms / 1000

def findEventTime(event: str, i: int):
    e = i
    while e < len(srt):
        vals = srt[e + 2].split("+")
        if event in vals:
            return parseTime(srt[e + 1][:13])
        e += 4
    raise RuntimeError(f"bruh there's no {event} past line {i}")


while i < len(srt):
    time = parseTime(srt[i + 1][:13])
    vals = srt[i + 2].split("+")

    vals = sorted(vals, key=lambda x: x == "RR" or x == "RL", reverse=True)

    for value in vals:
        if value == "RL":
            events.append(f"events::createFlip({time}, Rotation::COUNTERCWISE),\n")
        elif value == "RR":
            events.append(f"events::createFlip({time}, Rotation::CWISE),\n")
        elif value.startswith("HOLD"):
            if "END" not in value:
                direction = value[4:]
                if direction == "DOWN":
                    direction = "BOTTOM"
                if direction == "UP":
                    direction = "TOP"
                duration = findEventTime(value + "END", i) - time
                events.append(f"events::createArrowHold({time}, Direction::{direction}, {duration}),\n")
        elif value == "KICK":
            events.append(f"events::createKick({time}),\n")
        elif value == "SNARE":
            events.append(f"events::createSnare({time}),\n")
        elif value == "TOP" or value == "BOTTOM" or value == "LEFT" or value == "RIGHT" or value == "DOWN" or value == "UP":
            if value == "DOWN":
                value = "BOTTOM"
            if value == "UP":
                value = "TOP"
            events.append(f"events::createArrow({time}, Direction::{value}),\n")
        elif value.startswith("BLIND"):
            if "END" not in value:
                duration = findEventTime("BLINDEND", i) - time
                events.append(f"events::createBlindness({time}, {duration}),\n")
        else:
            errors.append(f"unknown event {value}")

    i += 4

result = "{\n"

result += "".join(events)

result += "}"

print(result)
print(errors)
