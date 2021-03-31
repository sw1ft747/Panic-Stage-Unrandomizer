int __userpurge CDirectorScriptedEventManager::StartPanicEvent@<eax>(CDirectorScriptedEventManager *a1@<ecx>, int a2@<ebx>, signed int a3, int a4)
{
    CDirectorScriptedEventManager *ScriptedEventManager; // esi
    float *v5; // edi
    int result; // eax
    CDirector *v7; // edi
    double v8; // st7
    float v9; // ST28_4
    float v10; // xmm0_4
    char *v11; // edi
    double v12; // st7
    float v13; // ST24_4
    signed int v14; // edi
    signed int v15; // eax
    bool v16; // zf
    int v17; // esi
    int v18; // esi
    int v19; // eax
    float v20; // [esp+28h] [ebp-4h]

    ScriptedEventManager = a1;
    if ( *(float *)&TheDirector->unknown872[20] > 0.0 )
    {
        v5 = (float *)&TheDirector->unknown872[12];

        if ( Countdown::Now() < v5[2] )
            return UTIL_LogPrintf( "%3.2f: Director::StartPanicEvent( %d ): ERROR: It is too soon to restart a PanicEvent.\n", *(float *)(gpGlobals + 12), a3);
    }

    UTIL_LogPrintf("%3.2f: Director::StartPanicEvent( %d ): Panic event started.\n", *(float *)(gpGlobals + 12), a3);

    v7 = TheDirector;
    v8 = Countdown::Now() + 3.0;

    if ( *(float *)&v7->unknown872[20] != v8 )
    {
        (**(void (__thiscall ***)(char *, char *))&v7->unknown872[12])(&v7->unknown872[12], &v7->unknown872[20]);
        v9 = v8;
        *(float *)&v7->unknown872[20] = v9;
    }

    v10 = *(float *)&v7->unknown872[16];
    v11 = &v7->unknown872[16];

    if ( v10 != 3.0 )
    {
        (**((void (__thiscall ***)(_DWORD *, char *))v11 - 1))((_DWORD *)v11 - 1, v11);
        *(_DWORD *)v11 = 1077936128;
    }

    *(_DWORD *)(TheZombieManager + 528) = 0;
    UTIL_LogPrintf("%3.2f: ZombieManager::ClearPendingMobCount: All mobs cleared.\n", *(float *)(gpGlobals + 12));

    *(_DWORD *)&ScriptedEventManager[1].m_bInSacrificeFinale = 0;

    v20 = RandomFloat(1.0, 2.0);
    v12 = Countdown::Now() + v20;

    // Установка времени таймера и его длительности
    if ( ScriptedEventManager[1].m_SacrificeEndScenarioTimer.m_timestamp != v12 )
    {
        ((void (__thiscall *)(CountdownTimer *, float *))ScriptedEventManager[1].m_SacrificeEndScenarioTimer.vptr->NetworkStateChanged)(
        &ScriptedEventManager[1].m_SacrificeEndScenarioTimer,
        &ScriptedEventManager[1].m_SacrificeEndScenarioTimer.m_timestamp);
        v13 = v12;
        ScriptedEventManager[1].m_SacrificeEndScenarioTimer.m_timestamp = v13;
    }
    if ( ScriptedEventManager[1].m_SacrificeEndScenarioTimer.m_duration != v20 )
    {
        ((void (__thiscall *)(CountdownTimer *, float *))ScriptedEventManager[1].m_SacrificeEndScenarioTimer.vptr->NetworkStateChanged)(
        &ScriptedEventManager[1].m_SacrificeEndScenarioTimer,
        &ScriptedEventManager[1].m_SacrificeEndScenarioTimer.m_duration);
        ScriptedEventManager[1].m_SacrificeEndScenarioTimer.m_duration = v20;
    }

    ScriptedEventManager[2].m_FinaleType = 0;

    UTIL_LogPrintf("%3.2f: Director: Panic Stage = STAGE_INITIAL_DELAY\n", *(float *)(gpGlobals + 12));

    v14 = a3;
    v15 = a3;

    if ( !a3 )  v15 = 1;

    v16 = LOBYTE(ScriptedEventManager[1].m_PanicEventStage) == 0;
    ScriptedEventManager[2].m_CurrentFinaleStage = v15;

    if ( v16 && !sub_1027A0D0() && CDirectorChallengeMode::AllowCrescendoEvents(&TheDirector->ChallengeModePtr->m_ChallengeModeActive) )
    {
        LOWORD(ScriptedEventManager[1].m_PanicEventStage) = 257;
        if ( !v14 )
        {
            if ( CDirector::GetMapArcValue((int)TheDirector) >= 2 )
            {
                ScriptedEventManager[2].m_CurrentFinaleStage = 2;
                UTIL_LogPrintf("%3.2f: Director::StartPanicEvent: Staring 2 wave panic event\n", *(float *)(gpGlobals + 12));
            }
            else
            {
                ScriptedEventManager[2].m_CurrentFinaleStage = 1;
                UTIL_LogPrintf("%3.2f: Director::StartPanicEvent: Staring 1 wave panic event\n", *(float *)(gpGlobals + 12));
            }
        }
    }

    v17 = TheZombieManager;
    V_memset((void *)(TheZombieManager + 80), 0, 0x18u);
    *(_DWORD *)(v17 + 104) = 0;
    result = CDirector::GetCommonInfectedLimit((int)TheDirector);

    if ( result )
    {
        result = (*(int (__stdcall **)(const char *, _DWORD, _DWORD))(*(_DWORD *)gameeventmanager + 24))(
                "create_panic_event",
                0,
                0);

        v18 = result;

        if ( result )
        {
            if ( a4 )
                v19 = (*(int (__stdcall **)(_DWORD))(*(_DWORD *)engine + 64))(*(_DWORD *)(a4 + 40));
            else
                v19 = 0;
            (*(void (__thiscall **)(int, const char *, int))(*(_DWORD *)v18 + 44))(v18, "userid", v19);
            result = (*(int (__stdcall **)(int, _DWORD))(*(_DWORD *)gameeventmanager + 28))(v18, 0);
        }
    }

    return result;
}

void __thiscall CDirectorScriptedEventManager::UpdatePanicEvents(int this)
{
    int v1; // edi
    int *v2; // eax
    int v3; // ebx
    signed int v4; // esi
    char **v5; // eax
    int v6; // eax
    int v7; // eax
    int v8; // eax
    char v9; // al
    double flTime; // st6
    double result; // st7
    bool v12; // al
    int v13; // ecx
    CDirector *v14; // esi
    float v15; // ST08_4
    float v16; // ST04_4
    float v17; // ST08_4
    char **i; // [esp+18h] [ebp-8h]
    bool v19; // [esp+1Fh] [ebp-1h]

    v1 = this;
    switch ( *(_DWORD *)(this + 296) ) // enum PanicEventStage
    {
        case STAGE_INITIAL_DELAY:
        {
            if ( Countdown::Now() >= *(float *)(this + 308) )
            {
                *(_DWORD *)(v1 + 296) = 1;
                DevMsg("STAGE_INITIAL_DELAY -> STAGE_MEGA_MOB\n");
                *(_DWORD *)(v1 + 312) = 0;
                CDirectorScriptedEventManager::PlayMegaMobWarningSounds();
            }
        }
        break;

        case STAGE_MEGA_MOB:
        {
            i = 0;
            v2 = TheNextBots();
            sub_1027C1F0((unsigned __int16 *)v2, (int)&i);

            if ( CDirector::GetMegaMobSize(TheDirector) > 1 )
                ZombieManager::SpawnMegaMob((void (__thiscall ***)(int, int))TheZombieManager);

            ++*(_DWORD *)(v1 + 312);
            *(_DWORD *)(v1 + 296) = 2;

            SetTimerDuration((void (__thiscall ***)(void *, int))(v1 + 300), 10.0);

            DevMsg("STAGE_MEGA_MOB -> STAGE_WAIT_FOR_COMBAT_TO_END\n");
        }
        break;

        case STAGE_WAIT_FOR_COMBAT_TO_END:
        {
            if ( *(_DWORD *)(TheZombieManager + 528) > 0 )
            {
                SetTimerDuration((void (__thiscall ***)(void *, int))(this + 300), 10.0);
                ZombieManager::ClearPendingMobCount(TheZombieManager);
            }
            if ( TheDirector->ChallengeModePtr->m_ChallengeModeActive )
            {
                v3 = 0;
                v4 = 20;
                v5 = &off_1078102C[-20];
                v19 = 0;

                for ( i = &off_1078102C[-20]; ; v5 = i )
                {
                    v6 = CDirector::GetScriptValue(TheDirector, (int)v5[v4], 0);
                    if ( v6 )
                    {
                        v7 = v6 - *(_DWORD *)(v4 * 4 + TheZombieManager);
                        v19 = 1;
                        v3 += v7 < 0 ? 0 : v7;
                    }

                    ++v4;

                    if ( v4 >= 26 )
                        break;
                }

                v8 = CDirector::GetScriptValue(TheDirector, (int)"TotalSpecials", 0);

                if ( v8 )
                {
                    v3 += (v8 - *(_DWORD *)(TheZombieManager + 104)) & ((v8 - *(_DWORD *)(TheZombieManager + 104) < 0) - 1);
                    v9 = 1;
                }
                else
                {
                    v9 = v19;
                }
                
                if ( v3 <= 0 )
                {
                    if ( v9 && CDirector::GetScriptValue(TheDirector, (int)"PanicSpecialsOnly", 0) )
                    {
                        if ( *(float *)(v1 + 308) != -1.0 )
                        {
                            (**(void (__thiscall ***)(int, int))(v1 + 300))(v1 + 300, v1 + 308);
                            *(_DWORD *)(v1 + 308) = -1082130432;
                        }

                        ZombieManager::ClearPendingMobCount2((_DWORD *)TheZombieManager);
                        DevMsg("Hoping to kick out of mode, finished the specials\n");
                    }
                }
                else
                {
                    SetTimerDuration((void (__thiscall ***)(void *, int))(v1 + 300), 10.0);
                }
            }

            flTime = Countdown::Now();
            result = *(float *)(v1 + 308);

            if ( flTime >= result )
            {
                v12 = CDirectorScriptedEventManager::ShouldPanicForever((CDirectorScriptedEventManager *)v1);
                v13 = *(_DWORD *)(v1 + 312);
                v19 = v12;
                if ( v13 < *(_DWORD *)(v1 + 316) || v12 )
                {
                    *(_DWORD *)(v1 + 296) = 3;
                    
                    v14 = TheDirector;
                    CDirector::GetPanicWavePauseMax(TheDirector);
                    v15 = result;
                    CDirector::GetPanicWavePauseMin(v14);
                    v16 = result;
                    v17 = RandomFloat(LODWORD(v16), LODWORD(v15)); // LOWDWORD => 0xFFFFFFFFFFFFFFFF & 0x00000000FFFFFFFF = 0x00000000FFFFFFFF
					
					// v17 = RandomFloat(CDirector::GetPanicWavePauseMin(TheDirector), CDirector::GetPanicWavePauseMax(TheDirector));

                    SetTimerDuration((void (__thiscall ***)(void *, int))(v1 + 300), v17);

                    DevMsg("STAGE_WAIT_FOR_COMBAT_TO_END -> STAGE_PAUSE\n");

                    if ( *(_DWORD *)v1 != 5 && !v19 )
                        CDirector::OnFinalePause(TheDirector);
                }
                else
                {
                    CDirector::OnMobRushStart(TheDirector);
                    *(_DWORD *)(v1 + 296) = 4;
                    CL4DGameStats::Event_PanicEventOver(&CL4DGameStats);
                    DevMsg("STAGE_WAIT_FOR_COMBAT_TO_END -> STAGE_DONE\n");
                    *(_BYTE *)(v1 + 273) = 0;
                    CDirectorScriptedEventManager::OnPanicEventFinished((CDirectorScriptedEventManager *)v1);
                }
            }
        }
        break;

        case STAGE_PAUSE:
        {
            if ( Countdown::Now() >= *(float *)(this + 308) && (TheDirector->m_iTempoState == TEMPO_BUILDUP || !CDirector::ShouldPausePanicWhenRelaxing(TheDirector)) )
            {
                *(_DWORD *)(v1 + 296) = 1;
                DevMsg("STAGE_PAUSE -> STAGE_MEGA_MOB\n");
            }
        }
        break;

        default: // >= STAGE_DONE
            return;
    }
}

void __thiscall ZombieManager::SpawnMegaMob(void (__thiscall ***this)(int, int))
{
    void (__thiscall ***v1)(int, int); // edi
    double v2; // st7
    float v3; // ST10_4
    float v4; // [esp+14h] [ebp-4h]

    v1 = this;

    v4 = RandomFloat(2.0, 4.0);
    v2 = Countdown::Now() + v4;

    // Установка времени таймера и его длительности
    if ( *((float *)v1 + 14) != v2 )
    {
        (*v1[12])((int)(v1 + 12), (int)(v1 + 14));
        v3 = v2;
        *((float *)v1 + 14) = v3;
    }
    if ( *((float *)v1 + 13) != v4 )
    {
        (*v1[12])((int)(v1 + 12), (int)(v1 + 13));
        *((float *)v1 + 13) = v4;
    }
}