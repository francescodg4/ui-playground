#pragma once

#include <QColor>
#include <QFont>

namespace proxmox {

/// Tokens of the Proxmox VE web console: light and flat, white panels on a pale-grey page,
/// 1px grey borders, one blue accent (the logo's orange is kept for the logo only). The dark
/// mode keeps the structure and the accent on dark-grey surfaces, like Proxmox VE's dark theme.
namespace Theme {

struct Colors {
    // surfaces
    QColor page; ///< --pve-page-bg
    QColor surface; ///< --pve-surface: panel bodies, grids, fields
    QColor surfaceAlt; ///< --pve-surface-alt: panel headers, grid headers
    QColor header; ///< --pve-header-bg: the header bar (menu bar)

    // lines
    QColor border; ///< --pve-border
    QColor buttonBorder; ///< --pve-button-border
    QColor borderHover;
    QColor checkBorder;
    QColor gridLine;

    // text
    QColor text; ///< --pve-text
    QColor textMuted; ///< --pve-text-muted
    QColor textPlaceholder; ///< --pve-text-placeholder
    QColor textDisabled;
    QColor check; ///< check marks and radio dots

    // accent
    QColor accent; ///< --pve-accent
    QColor accentHover; ///< --pve-accent-hover
    QColor accentPressed;
    QColor accentDisabled;

    // interaction
    QColor hover; ///< --pve-hover: tree and grid rows
    QColor menuHover; ///< --pve-menu-hover
    QColor treeSelected; ///< --pve-tree-selected
    QColor button; ///< toolbar button fill
    QColor buttonHover;
    QColor buttonPressed;
    QColor buttonDisabled;

    // data
    QColor barFill; ///< --pve-bar-fill
    QColor barTrack; ///< --pve-bar-track

    // scroll bars
    QColor scrollTrack;
    QColor scrollThumb;
    QColor scrollThumbHover;
    QColor scrollArrow;

    // tooltip
    QColor tooltip;
    QColor tooltipText;
    QColor tooltipBorder;
};

/// The Crisp theme of the skill.
const Colors& light();
/// Proxmox VE's dark theme.
const Colors& dark();

inline constexpr qreal radiusControl = 2; ///< --pve-radius-control

/// --pve-font-ui (Helvetica / Arial / Liberation Sans) at @p pointSize.
QFont font(qreal pointSize = 9.75);

} // namespace Theme

} // namespace proxmox
