import re


# -------------------------------------------------
# Common human abbreviations
# -------------------------------------------------

WORD_REPLACEMENTS = {

    # Time
    "mins": "minutes",
    "min": "minutes",
    "mins.": "minutes",
    "min.": "minutes",

    "hrs": "hours",
    "hr": "hours",
    "hrs.": "hours",
    "hr.": "hours",

    "secs": "seconds",
    "sec": "seconds",
    "secs.": "seconds",
    "sec.": "seconds",

    # Days
    "mon": "monday",
    "mon.": "monday",

    "tue": "tuesday",
    "tues": "tuesday",
    "tue.": "tuesday",
    "tues.": "tuesday",

    "wed": "wednesday",
    "wed.": "wednesday",

    "thu": "thursday",
    "thur": "thursday",
    "thurs": "thursday",
    "thu.": "thursday",
    "thur.": "thursday",
    "thurs.": "thursday",

    "fri": "friday",
    "fri.": "friday",

    "sat": "saturday",
    "sat.": "saturday",

    "sun": "sunday",
    "sun.": "sunday",

    # Common date words
    "tmrw": "tomorrow",
    "tmr": "tomorrow",
    "tom": "tomorrow",

    # Time of day
    "a.m.": "am",
    "p.m.": "pm",
}


def normalize_text(text):

    text = text.lower().strip()

    # Remove unnecessary punctuation around words
    text = re.sub(
        r"([a-z]+)\.",
        r"\1",
        text
    )

    # Split into words
    words = text.split()

    normalized_words = []

    for word in words:

        replacement = WORD_REPLACEMENTS.get(
            word,
            word
        )

        normalized_words.append(
            replacement
        )

    normalized_text = " ".join(
        normalized_words
    )

    # Clean repeated spaces
    normalized_text = re.sub(
        r"\s+",
        " ",
        normalized_text
    ).strip()

    return normalized_text